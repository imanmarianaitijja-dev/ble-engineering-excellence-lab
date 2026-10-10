# AI-Assisted Testing Strategy: Human-in-the-Loop Framework

## Objective
Demonstrate how an embedded software engineering team can responsibly leverage Large Language Models (LLMs) to enhance test coverage for state-machine robustness without introducing non-deterministic behavior or unverified assumptions into firmware C code.

---

## 1. Practical Case Study & Human Validation

### Method
1. Provide the LLM with state enum definitions (`ble_ll_state_t` and `ble_ll_event_t`).
2. Request edge-case event sequences for invalid state transitions.
3. Review proposals for protocol alignment before converting accepted cases into C unit tests.

### AI Proposals vs. Engineering Review

| AI Proposal | Review Assessment | Decision |
| :--- | :--- | :--- |
| `BLE_LL_EVENT_DISCONNECT` during `ADVERTISING` state | Invalid event in advertising context. Useful to test state isolation. | **Accepted** -> Added to robustness tests |
| `BLE_LL_EVENT_CONNECT_REQ_RECEIVED` in `INITIATING` state | Invalid. An Initiator sends `CONNECT_REQ`; receiving it belongs to Scanning/Peripheral role. | **Accepted** -> Added to robustness tests |
| Requesting `START_ADV` while in `CONNECTION` state | Multi-role scheduling is out of scope for this single-link model. | **Rejected** (Scope boundary) |

---

## 2. Productivity Measurement Strategy

> **Engineering Principle:** No productivity metrics or time-saving claims are declared without an empirical baseline.

### Evaluation Protocol
* **Baseline:** Track time required for an engineer to manually draft edge-case unit test suites.
* **AI-Assisted:** Measure total time for LLM prompt generation + mandatory human review + test integration.
* **Quality Gate:** Measure defect detection rate via fault-injection (mutation testing) in CI.

---

## 3. Risk Mitigation & Security Guardrails

To prevent intellectual property leaks, security vulnerabilities, or regression injection, the following rules apply:

* **No Proprietary Code Leakage:** Never input unannounced hardware register maps, private cryptographic keys, or proprietary protocol stack implementations into public LLM endpoints.
* **Hallucination Containment:** Generated unit tests must compile with strict compiler flags (`-Wall -Wextra -Werror`) and pass static analysis (`cppcheck`) before merging into `main`.
* **Verification Boundary:** An AI-generated test is considered invalid until executed and verified by the automated build pipeline (`test_runner`).