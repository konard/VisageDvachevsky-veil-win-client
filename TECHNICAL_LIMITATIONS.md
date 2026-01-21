# Technical Limitations and Improvement Roadmap

This document tracks architectural limitations and potential improvements for VEIL that don't contradict its core DPI evasion mission.

---

## 🚨 Critical: DPI Evasion Gaps

### Issue #1: WebSocket Wrapper Without HTTP Handshake

**Severity:** High
**Component:** Protocol Wrappers (QUIC-Like mode)
**File:** `src/common/protocol_wrapper/websocket_wrapper.cpp`

**Description:**
QUIC-Like mode wraps packets in RFC 6455 WebSocket binary frames, but skips the HTTP Upgrade handshake. Advanced DPI can detect this anomaly.

**Current Behavior:**
```
Client → Server: [WebSocket Binary Frame]  (no HTTP handshake)
```

**Expected Legitimate Traffic:**
```
Client → Server: GET /path HTTP/1.1
                 Upgrade: websocket
Server → Client: HTTP/1.1 101 Switching Protocols
[WebSocket frames begin]
```

**Impact:**
- DPI rule: "WebSocket frames without HTTP = VPN"
- Easy detection despite correct frame structure

**Proposed Solution:**
Add optional HTTP-over-UDP handshake emulation:
1. First packet: fake HTTP Upgrade request
2. Second packet: fake 101 response
3. Subsequent packets: WebSocket frames (as now)

**Implementation:**
- Add `HttpHandshakeEmulator` class
- Config flag: `enable_http_handshake_emulation`
- Overhead: 2 extra packets per connection (~1KB)

**Related:** `docs/protocol_wrappers.md:254-257`

---

### Issue #2: Missing TLS Record Layer Wrapper

**Severity:** High
**Component:** Protocol Wrappers
**File:** New implementation needed

**Description:**
Real WebSocket almost always runs over TLS (wss://). Plain ws:// over UDP is extremely rare, creating easy DPI signature.

**Current Behavior:**
```
[UDP Header][WebSocket Frame][ChaCha20-Poly1305 ciphertext]
```

**Expected (wss:// equivalent):**
```
[UDP Header][TLS Record][Application Data (WebSocket frame)]
```

**Impact:**
- DPI filter: "WebSocket without TLS = block"
- Missing from legitimate traffic patterns

**Proposed Solution:**
Implement TLS 1.3 record wrapper (already planned in Future Wrappers):
- Wrap packets in TLS application data records
- Overhead: 5-20 bytes per record
- New mode: `ProtocolWrapperType::kTLS`

**References:**
- RFC 8446 (TLS 1.3)
- `docs/protocol_wrappers.md:271-276`

---

### Issue #3: Predictable Session Rotation Interval

**Severity:** Medium
**Component:** Session Management
**File:** `src/common/session/session_rotator.cpp`

**Description:**
Session ID rotates every exactly 30 seconds. ML-based DPI can detect this perfect periodicity.

**Current Behavior:**
```
Session timings: [0s] → [30s] → [60s] → [90s] → [120s]
                  ↑      ↑       ↑       ↑       ↑
              Perfect 30s intervals = automation detected
```

**Real P2P Traffic:**
```
Connection durations: 13s, 2m47s, 38s, 5m19s, 1m02s
                      ↑ Irregular, human-like patterns
```

**Impact:**
- ML classifier feature: "Std deviation of session duration < threshold"
- Long-term observation reveals pattern

**Proposed Solution:**
Add exponential jitter to rotation interval:

```cpp
duration compute_rotation_interval() {
  const duration base = 30s;
  const duration jitter_range = 10s;

  // Exponential distribution: most intervals ~30s, some much longer
  double u = uniform_random(0.0, 1.0);
  double jitter = -log(u) * jitter_range.count();
  jitter = clamp(jitter, -10.0, 20.0);  // Range: 20-50s

  return base + seconds(static_cast<int>(jitter));
}
```

**Complexity:** Low (1-2 hour implementation)

---

## ⚡ Performance Bottlenecks

### Issue #4: Single-Threaded Architecture Limits Throughput

**Severity:** Medium
**Component:** Core Architecture
**File:** `src/transport/event_loop/`

**Description:**
All I/O processing happens on single main thread. Target performance is 500 Mbps, vs 10+ Gbps for multi-threaded VPNs.

**Current Architecture:**
```
Main Thread:
  ├─ UDP socket polling
  ├─ TUN device I/O
  ├─ Encryption/decryption
  ├─ Retransmit processing
  └─ Session management

Result: 1 CPU core used, 7-63 cores idle
```

**Comparison:**
| Protocol | Architecture | Throughput |
|----------|--------------|------------|
| WireGuard | Kernel module, multi-core | 10+ Gbps |
| OpenVPN | Multi-process | 1-2 Gbps |
| **VEIL** | Single-threaded | **500 Mbps** |

**Impact:**
- Can't handle 100+ clients at high speeds
- 95% of server hardware unused
- Higher costs (need more servers)

**Proposed Solution (Multi-Phase):**

**Phase 1: Pipeline Parallelism**
```
Thread 1 (RX):      UDP receive → decrypt
       ↓ (lock-free queue)
Thread 2 (Process): Reassembly → routing
       ↓ (lock-free queue)
Thread 3 (TX):      Encrypt → UDP send
```
Target: 1-2 Gbps

**Phase 2: Per-Client Threading**
```
SO_REUSEPORT + thread pool
Each client session → dedicated thread
```
Target: 3-5 Gbps

**Concerns:**
- May affect obfuscation timing precision
- Increased complexity (thread safety)
- Trade-off: performance vs stealth

**Priority:** Medium (needs careful evaluation)

---

### Issue #5: No 0-RTT Resumption

**Severity:** Low
**Component:** Handshake
**File:** `src/common/handshake/handshake_processor.cpp`

**Description:**
Client must complete 1-RTT handshake before sending data. Modern protocols support 0-RTT for returning clients.

**Current:**
```
RTT = 200ms (international)

Connection:     200ms (INIT → RESPONSE)
First request:  200ms
Total:          400ms
```

**With 0-RTT:**
```
Connection + data: 200ms (1 RTT)
Total:             200ms (50% faster)
```

**Proposed Solution:**
- Issue session tickets after first handshake
- Client caches ticket
- On reconnect: send ticket + encrypted data in INIT
- Server validates ticket and processes data immediately

**Security Notes:**
- 0-RTT vulnerable to replay (need anti-replay token)
- Only for idempotent operations
- Document risks clearly

**Priority:** Low (minor UX improvement)

---

## 🔐 Scalability Limitations

### Issue #6: PSK Authentication Doesn't Scale

**Severity:** Medium
**Component:** Authentication
**File:** `src/common/handshake/`

**Description:**
Single Pre-Shared Key (PSK) for all clients limits scalability and granular access control.

**Problems:**

**1. No Individual Revocation:**
```
WireGuard:
  Remove client public key → access revoked immediately

VEIL:
  Client knows PSK → must generate new PSK for ALL clients
  → 1000 clients must all update simultaneously
```

**2. Key Distribution:**
- How to securely give PSK to new client?
- No scalable key distribution mechanism

**3. Compromise Impact:**
- One client compromised = PSK leaked
- All clients must rotate keys

**Proposed Solutions:**

**Option A: Per-Client PSK** (simpler)
```cpp
// Server config
client_keys = {
  "alice": "psk_alice_...",
  "bob":   "psk_bob_...",
}

// Client sends client_id in INIT
// Server looks up corresponding PSK
```

**Option B: Public-Key Auth** (better, like WireGuard)
```
Client generates keypair
Server has list of authorized public keys
Can revoke individual clients
```

**Complexity:** Very High (protocol change)
**Priority:** Low (acceptable for small deployments <100 clients)

---

## 🧪 Testing Gaps

### Issue #7: No Machine Learning Based DPI Testing

**Severity:** High
**Component:** Testing
**File:** New test suite needed

**Description:**
VEIL validated against signature-based DPI (nDPI) and statistical analysis, but NOT against ML-based classifiers used by modern DPI systems (GFW, TSPU).

**Current Testing:**
- ✅ nDPI (signature-based)
- ✅ Entropy analysis
- ✅ Packet size distribution
- ❌ Machine learning classifiers

**Risk:**
ML models can detect subtle patterns:
- HMAC-based deterministic padding (mathematical pattern)
- Exponential distribution heartbeats (model signature)
- Session rotation patterns (periodicity)

**Example ML Detection:**
```python
features = extract_features(traffic):
  - Packet size histogram (100 bins)
  - Inter-arrival time autocorrelation
  - Burst patterns (packets/second over time)
  - Entropy time series
  - Session duration distribution

classifier = RandomForest(features, n_estimators=100)
if classifier.predict(features) == "VPN":
  block()
```

**Proposed Solution:**

**Phase 1: Build Dataset**
1. Collect VEIL pcaps (all 4 modes, 10+ hours each)
2. Collect legitimate traffic:
   - IoT devices (smart home sensors)
   - WebSocket apps (chat, gaming)
   - Video streaming
   - Web browsing

**Phase 2: Train Classifiers**
1. Extract features (100+ dimensions)
2. Train multiple models:
   - Random Forest
   - Gradient Boosting (XGBoost)
   - Neural Network (LSTM for time series)
3. Measure detection accuracy

**Phase 3: Iterate**
- If detection >5% → improve obfuscation
- Adversarial training loop
- Repeat until <5% false positive AND <5% false negative

**Tools:**
- Python: scikit-learn, TensorFlow
- Feature extraction: tshark, Python scapy
- Benchmark: VEIL vs Shadowsocks vs V2Ray

**Complexity:** High (requires ML expertise)
**Priority:** **Critical** (unknown real-world effectiveness)

---

## 📋 Summary & Priorities

| Issue | Impact | Complexity | Priority | Est. Effort |
|-------|--------|-----------|----------|-------------|
| #1 WebSocket w/o HTTP | Detection risk | Medium | **High** | 1-2 weeks |
| #2 No TLS wrapper | Detection risk | High | **High** | 3-4 weeks |
| #3 Fixed rotation time | ML detection | Low | Medium | 1 day |
| #4 Single-threaded | Performance | Very High | Medium | 2-3 months |
| #5 No 0-RTT | UX latency | High | Low | 2-3 weeks |
| #6 PSK auth | Scalability | Very High | Low | 1-2 months |
| #7 No ML testing | Unknown risk | High | **Critical** | 4-6 weeks |

---

## Roadmap Recommendation

**Immediate (Next Release):**
1. Issue #3: Add jitter to session rotation (1 day)
2. Issue #7: Begin ML-based DPI testing (foundational)

**Short-term (1-3 months):**
1. Issue #1: HTTP handshake emulation (critical for QUIC-Like mode)
2. Issue #2: TLS record wrapper (major DPI evasion improvement)
3. Issue #7: Complete ML testing and iterate on obfuscation

**Long-term (3-6 months):**
1. Issue #4: Multi-threaded architecture (if performance becomes bottleneck)
2. Issue #6: Per-client authentication (if scaling beyond 100 clients)
3. Issue #5: 0-RTT (nice-to-have UX improvement)

---

## Non-Issues (Design Trade-offs)

These are **intentional** design decisions, not problems:

✅ **Padding overhead (20-40%)**
→ Necessary cost for DPI evasion, defeats size-based fingerprinting

✅ **Trickle mode slow (10-50 kbps)**
→ Designed for extreme stealth in high-censorship environments, not general use

✅ **ChaCha20-Poly1305 vs AES-GCM**
→ Better for ARM devices (3x faster), proven security, used in WireGuard

✅ **UDP-based (vs TCP)**
→ Avoids TCP-over-TCP problems, better for VPN tunneling

✅ **C++20 complexity**
→ Performance critical, needs low-level control, setup wizard mitigates UX issues

---

## Contributing

To work on any of these issues:

1. Comment on this document to claim an issue
2. Create feature branch: `feature/issue-N-description`
3. Reference this document in PR
4. Update this doc when issue is resolved

**Questions?** Open discussion in GitHub Discussions or Telegram.
