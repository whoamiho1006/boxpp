# Networking library.
POSIX Socket wrapper library.

## 1. TODO List.
	* DNS Resolver and POSIX Name-resolver.
	* Scope-ID enumerator for IPv6.

## 2. Supports
	* TCP/UDP over IPv4/IPv6.
	* Poll() method.

## 3. Network Address and Resolvers.
### Resolvers are now WIP.
### FIPAddress
FIPAddress represents IPv4 addresses including Masking address like 8.8.8.0/24.
And, this class contains 4 bytes-- network-endian -- address and its width for masking purpose.

* Constructors:
```
	// Anycast address (INADDR_IN)
	FIPAddress(__AnyType = Any);

	// Loopback address (127.0.0.1)
	FIPAddress(__LoopbackType);
	
	// IP Address Parser (from string)
	FIPAddress(const ansi_t* Address);
	FIPAddress(const wide_t* Address);

	// IP Parser, but masking address explicitly.
	FIPAddress(__MaskType, const ansi_t* Address, s8 Width);
	FIPAddress(__MaskType, const wide_t* Address, s8 Width);
```

* Conversions:
```
	/*
		IP Address to boolean.
		returns true for almost address excluding parsing error.
	*/
	operator bool() const;

	// Stringifiers.
	bool ToString(FWideString& OutString) const;
	bool ToString(FAnsiString& OutString) const;
	FString ToString() const;
```

* Getters:
```
	bool IsAnycast() const;
	bool IsLoopback() const;
	bool IsMasking() const;
	u8 GetWidth() const;

	u32 GetDword() const;
	u8 GetByteAt(s8 Offset) const;
```

* Setters:
```
	void SetDword(u32 Value);
	void SetByteAt(s8 Offset, u8 Value);
	void SetWidth(u8 Width);
```

### FIPAddressV6
FIPAddress represents IPv6 addresses, but masking feature isn't implemented yet.
And, this class contains 16 bytes-- network-endian -- address and 4 bytes scope-id.

* Constructors:
```
	// Anycast address (INADDR_IN)
	FIPAddressV6(__AnyType = Any);

	// Loopback address (::1)
	FIPAddressV6(__LoopbackType);
	
	/*
	IPv6 Address Parser (from string)
		* Scope-ID notation is supported on generic operating systems.
	*/
	FIPAddressV6(const ansi_t* Address);
	FIPAddressV6(const wide_t* Address);
```
	
* Conversions:
```
	/*
		IPv6 Address to boolean.
		returns true for almost address excluding Anycast address.
	*/
	operator bool() const;

	// Stringifiers.
	bool ToString(FWideString& OutString) const;
	bool ToString(FAnsiString& OutString) const;
	FString ToString() const;
```

* Getters:
```
	bool IsAnycast() const;
	bool IsLoopback() const;

	u32 GetQword(u32 Index) const;
	u8 GetByteAt(s8 Offset) const;
	u32 GetScopeId() const;
```

* Setters:
```
	void SetQword(u32 Index, u32 Value);
	void SetByteAt(s8 Offset, u8 Value);
	void SetScopeId(u32 Value);
```

### TIPEndpoint<ADDR_TYPE> and its aliases.
TIPEndpoint<> template combines IP Address and port number for TCP/UDP.
And, this template works only with IPv4 and IPv6 address.

* Constructors:
```
	TIPEndpoint();

	// Parse from string with listen-address option as parsing hint.
	TIPEndpoint(const ansi_t* InString, bool bToListen = false);
	TIPEndpoint(const wide_t* InString, bool bToListen = false);

	// Wrapping constructor.
	TIPEndpoint(const AddressType& Address, s32 Port);
	TIPEndpoint(AddressType&& Address, s32 Port);
```
	
* Conversions:
```
	/*
		Simply, ( Address && Port ).
		In almost case, returns true excluding parsing error and no-port.
		and, in IPv6, returns false if the address is Anycast address.

		** no-port: port number is less or equal than zero.
	*/
	operator bool() const;

	/*
		Stringifiers.

		1. IPv4 address will be stringified like:
			-> aaa.bbb.ccc.ddd:port.
				e.g. 8.8.8.8:52

			-> aaa.bbb.ccc.ddd (if no-port)
				e.g. 8.8.4.4

		2. IPv6 address will be stringified like:
			-> [aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh]:port
				e.g. [::1]:8080

			-> aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh (if no-port)
				e.g. ::1

		** no-port: port number is less or equal than zero.
	*/
	bool ToString(FWideString& OutString) const;
	bool ToString(FAnsiString& OutString) const;
	FString ToString() const;
```
	
* Getters:
```
	const AddressType& GetAddress() const;
	s32 GetPort() const;
```

* Setters:
```
	void SetAddress(const AddressType& Addr);
	void SetAddress(AddressType&& Addr);
	void SetPort(s32 Port);
```
	
## 4. Socket API
### EProtocolType
```
	Inet,
	Inet6
```

### ESocketType
```
	Tcp,
	Udp
```

### ESocketChannel
```
	ESOCK_NONE = 0,
	ESOCK_INPUT = 1,
	ESOCK_OUTPUT = 2,
	ESOCK_ERROR = 4,
	ESOCK_HANGUP = 8
```

### FSocket
FSocket is wrapper of FSocketLayer. (in detail, FSocketLayer is also wrapper)
and, FSocket instances are not copy-constrctible. move constructor only supported.
The names of methods are almost identical to the POSIX methods.

* Constructors:
```
	FSocket();
	FSocket(EProtocolType Protocol, ESocketType SocketType);
```
	
* Conversions:
```
	// returns false for invalid socket.
	operator bool() const;
```

* Methods:
```
	// This method determines the latest operation was successful or not.
	bool HasError() const;
	ESocketError GetError() const;

	/*
		If the socket is non-blocking socket,
		some actions will not block, so, it returns immediately with return-zero.
		in that case, this method will return true.
		otherwise, returns false.
	*/
	bool ShouldAgain() const;

	// Get socket's endpoint.
	bool GetSockName(FIPAddress& Address, s32& Port) const;
	bool GetSockName(FIPAddressV6& Address, s32& Port) const;
	bool GetSockName(TIPEndpoint<AddressType>& Endpoint) const;

	// Determines this socket is non-blocking socket or not.
	bool IsNonblock() const;
	bool SetNonblock(bool v);

	// Determines Nagle algorithm is enabled or not.
	bool IsNoDelay() const;
	bool SetNoDelay(bool v);

	s32 GetReceiveTimeout() const;
	bool SetReceiveTimeout(s32 Timeout);

	s32 GetSendTimeout() const;
	bool SetSendTimeout(s32 Timeout);
	
	bool Bind(const FIPAddress& Address, s32 Port);
	bool Bind(const FIPAddressV6& Address, s32 Port);
	ssize_t Bind(const TIPEndpoint<AddressType>& Endpoint);

	// TCP Listen.
	bool Listen(s32 Backlog);

	/*
		Tests the newbie is now accept-pending or not for server-mode socket.
		And, Receivable bytes are ready or not for client-mode socket.

		This method is identical same:
			Poll(ESOCK_INPUT, 0) == ESOCK_INPUT
	*/
	bool Pending();

	// TCP Accept.
	bool Accept(FSocket& Newbie);
	
	/*
		Connect to remote server.
		Timeout <  0: blocked during connected or error.
		Timeout >= 0: blocked during specified timeout.
	*/
	bool Connect(const FIPAddress& Address, s32 Port, s32 Timeout = -1);
	bool Connect(const FIPAddressV6& Address, s32 Port, s32 Timeout = -1);
	ssize_t Connect(const TIPEndpoint<AddressType>& Endpoint, s32 Timeout = -1);

	bool Shutdown(bool Rd = true, bool Wr = true);

	// TCP Recv/Send.
	ssize_t Receive(void* Buffer, size_t Size);
	ssize_t Send(const void* Buffer, size_t Size);

	// UDP Recv/Send.
	ssize_t SendTo(const FIPAddress& To, const u16& LocalPort, const void* Buffer, size_t Size);
	ssize_t SendTo(const FIPAddressV6& To, const u16& LocalPort, const void* Buffer, size_t Size);
	ssize_t SendTo(const TIPEndpoint<AddressType>& Endpoint, const void* Buffer, size_t Size);

	ssize_t ReceiveFrom(FIPAddress& From, u16& LocalPort, void* Buffer, size_t Size);
	ssize_t ReceiveFrom(FIPAddressV6& From, u16& LocalPort, void* Buffer, size_t Size);
	ssize_t ReceiveFrom(TIPEndpoint<AddressType>& Endpoint, void* Buffer, size_t Size)

	// Poll(...) function for single socket.
	ESocketChannel Poll(ESocketChannel Channels, const u32 Timeout = 0);

	// Close the socket.
	bool Close();

	/*
		Polling function.
		This method performs polling for specified channel and removes not-evented sockets.
	*/
	static ssize_t Poll(TArray<FSocket*>& Sockets, ESocketChannel Channels, const u32 Timeout = 0);
```

### FSocketLayer
```
This class contains only static methods. and they are samethings described above in FSocket.
```

### ESocketError
Some error-codes never used on crossing-env.
e.g. ESocketError::Pending is only for Windows.
e.g. ESocketError::NotSupportedSocket and HostDown is only for POSIX Systems.
refer details on SOCKET_GetErrorCodeFromRaw() function that declared at boxpp/base/network/SocketLayer.cpp.

```
	Success = 0,

	/* Non-blocking response codes. */
	Pending,
	WouldBlock, // == Pending if PLATFORM_WINDOWS || (PLATFORM_POSIX && EAGAIN != EWOULDBLOCK)
	InProgress,
	Incompleted, // == InProgrss if not PLATFORM_WINDOWS.

	/* Already doing something */
	Already,
	Cancelled,

	/* Interrupted. */
	Interrupted,

	/* Not wrapped error. */
	NotWrapped,

	/* Handle, OS related errors. */
	BadSocket,
	NoResources, // = TooManyRefs, TooManySockets, TooManyProcesses

	/* Creation error codes. */
	BadAddress,
	NoDestination,
	AddressInUse,
	AddressNotAvailable,
	InvalidProtocol,
	InvalidProtocolOptions,
	InvalidArgument,

	/* Requests about unsupported features... */
	NotSupportedProtocol,
	NotSupportedSocket,
	NotSupportedOperation,
	NotSupportedFamily,

	/* Host system related error codes. */
	HostDown,
	HostUnreachable,
	HostNotFound, // => Windows only.

	/* Network related error codes. */
	NetworkDown,
	NetworkUnreachable,
		
	/* Error codes before connection. */
	Aborted,
	Refused,
	NotConnected,

	/* Error codes after connection. */
	NoBuffer,
	Timedout,
	Shutdown,
	ResetByPeer,
	ResetByNetwork,
	ConnectedAlready
```

## 4-1. Wrappers.
### FTcpListener
Simple usage:
```
FTcpListener Server(EProtocolType::Inet, 8000);
BOX_ASSERT(Server.Start(), "Start() failed");

while (true) {
	FTcpClient Newbie;

	if (!Server.IsPending()) {
			
		continue;
	}

	if (Server.Accept(Newbie)) {
		FIPEndpoint Endpoint;
		const char* Packet = "HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=utf-8\r\n"
			"Content-Length: 20\r\n\r\n"
			"abcdabcdabcdabcdabcd\r\n";

		if (Newbie.GetSocket()->GetSockName(Endpoint)) {
			printf("%S\n", Endpoint.ToString().GetRaw());
		}

		Newbie.Send(Packet, TNativeString<char>::Strlen(Packet));
		Newbie.Disconnect();
	}
}
```

### FTcpClient

## 5. Usage
### Server socket (simple http relay):
```
	FSocket Test(EProtocolType::Inet, ESocketType::Tcp);

	BOX_ASSERT(Test.Bind(FIPAddress::Any, 8000), "Bind() failed");
	BOX_ASSERT(Test.Listen(30), "Listen() failed");

	while (!Test.HasError()) {
		FSocket Newbie;

		if (Test.Accept(Newbie)) {
			FIPEndpoint Endpoint;
			const char* Packet = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html; charset=utf-8\r\n"
				"Content-Length: 20\r\n\r\n"
				"abcdabcdabcdabcdabcd\r\n";

			if (Newbie.GetSockName(Endpoint)) {
				printf("%S\n", Endpoint.ToString().GetRaw());
			}

			Newbie.Send(Packet, TNativeString<char>::Strlen(Packet));
			Newbie.Shutdown();
		}
	}
```

### Client socket:
