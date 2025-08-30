# TFTP Library {#mainpage}

This library provides a TFTP Server and Client implementation.

Included within this Library is an example CLI TFTP Client and TFTP Server application.

This tool suite contains the components:
- TFTP Protocol
  - TFTP core library (@ref Tftp):
    a portable RFC-compliant implementation (RFC 1350, 2347, 2348, 2349) providing client and server functionality, option negotiation (blksize/tsize/timeout), retransmission/timeout handling, and extensible I/O integration.
  - TFTP Qt library (@ref TftpQt):
    Qt-friendly wrappers around the core with signal/slot-based asynchronous APIs, Qt data types, and utilities to integrate TFTP client/server operations into Qt event loops and applications.
  - @subpage tftp_applications,
- %Helper Library
  - %Helper Library Namespace @ref Helper,
  - %Helper Qt Framework Namespace @ref HelperQt,
  - @subpage helper_applications.
