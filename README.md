Database
- Designed for multithread applications
- Uses my DbField, QueryObject and SafeQueue class

TCP/IP connection manager
- I make child classes of my SocketMgr interface, this allowed me to have consistent design for the two applications in my project
- TcpListener is a very generic class, can be used independent of anything lsee

Util.h
- Has various helper function my application uses, the most relative functions are sendBytes/recvBytes

# Packet class example
```
Packet packet;
uint64 playerGuid;

try
{
	packet >> playerGuid;
}
catch (STLBuffer::StlBufferException&)
{
	// Do something about it!
	printf("Oh no...\n");
}
```

# Database example
```
Database GameDb;

if (!GameDb.Initialize("host;port;user;pw;dbname"))
{
	printf("GameDb.Initialize - Cannot connect to the game database!");
	return 0;
}

// Example blocking query
if (std::shared_ptr<QueryResult> result = GameDb.Query("SELECT entry, name FROM table"))
{
	do
	{
		DbField* pFields = result->fetchCurrentRow();    
		const unsigned int entry = pFields[0].getUInt32();
		const std::string name = pFields[1].getCppString();
	}
	while (result->NextRow());
}

// If you wan't to issue a non blocking query without concern for the result, you queue it as such.
// The queue is executed in the order it's given queries, but is asynchronous to executions outside of that queue.
GameDb.QueueExecuteQuery("UPDATE table SET name = ''");

// Executes a blocking query without concern for the result.
GameDb.ExecuteQueryInstant("UPDATE table SET );

// If you want to set-up adding many queries to the queue at once with the option to cancel before you've finished adding them all in.
GameDb.BeginManyQueries();

// Would have many calls of 'QueueExecuteQuery' inside SaveAllPlayers().
if (Game::SaveAllPlayers())
{
	GameDb.CommitManyQueries();
}
else
{
	printf("Failed to save all players!");
	GameDb.CancelTransaction();
}

// If you want to get data without blocking, you queue up what I called a "Callback" by providing an ID and a query string.
// Then, later, check and process any results.
// GameDb.queueCallbackQuery(GET_PLAYER_DATA_QUERY, "SELECT * FROM players WHERE name = ''");
// GameDb.GrabAndClearCallbackQueries(uoPlaceToPutResults);
// ProcessResults(uoPlaceToPutResults);

// Cleanup
GameDb.Uninitialise();
```

# Example of how to use my SocketMgr interface
```
class ExampleSocketMgr : public SocketMgr
{
	public:
		ExampleSocketMgr(const uint32 listenSocketNumber) :
			m_uiListenSocket(listenSocketNumber)
		{
			startThreads();
		}

		// No need for this, SocketMgr cleans up the threads
		//~ExampleSocketMgr()

	private:
		void ListenThread() final
		{
			TcpListener listener(m_uiListenSocket);
			//setListenSocket(listener.mySocket());

			while (!doCancelListener())
			{
				std::string resultAddr;
				SOCKET sSocket = listener.waitAndAccept(resultAddr, true);

				if (!sSocket)
					continue;

				if (sSocket == INVALID_SOCKET)
					break;

				m_mySockets.push(sSocket);
			}

			printf("Exiting ListenThread.\n");
		}

		void WorkerThread() final
		{
			while (!doCancelWorker())
			{
				std::vector<SOCKET> mySockets;
				m_mySockets.popAll(mySockets);

				if (mySockets.empty())
				{
					// What's the hurry if there's nothing to do?
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					continue;
				}

				auto itr = mySockets.begin();

				while (itr != mySockets.end())
				{
					// Do something with each socket, like peak for data, get some bytes, that kind of stuff, its up to the user

					SocketPacket packet;
					packet.RecvSelf(*itr);

					// Example of how you get data out of it
					{
						uint64 playerGuid;

						try
						{
							packet >> playerGuid;
						}
						catch (STLBuffer::StlBufferException&)
						{
							// Do something about it!
							printf("Oh no...\n");
							itr = mySockets.erase(itr);
							continue;
						}
					}

					++itr;
				}

				// This is why my SafeQueue class isn't ideal here, 
				//  I would make up a "SafeList" or something if I was going to structure a child of SocketMgr this way
				m_mySockets.pushMany(mySockets);
			}

			printf("Exiting WorkerThread.\n");
		}

		const uint32 m_uiListenSocket;
		SafeQueue<SOCKET> m_mySockets;
};

// Make the object
ExampleSocketMgr mgr(12345);

// Do stuff
;

// Done!
mgr.Uninitialise();
```