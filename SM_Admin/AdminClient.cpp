#include "AdminClient.h"



AdminClient::AdminClient()
{
}


AdminClient::~AdminClient()
{
}


void AdminClient::connectToServer()
{

	while (1) {


		hPipe = CreateFile(
			pipeNameAdmin,
			GENERIC_READ |
			GENERIC_WRITE,
			0 | FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0 | FILE_FLAG_OVERLAPPED,
			NULL);

		if (hPipe != INVALID_HANDLE_VALUE) {
			break;
		}


		if (GetLastError() != ERROR_PIPE_BUSY) {
			DWORD i = GetLastError();
			_tprintf(TEXT("\nCreate file deu erro e nao foi BUSY. Erro = %d\n"), GetLastError());
			

		}

		if (!WaitNamedPipe(pipeNameAdmin, 30000)) {
			_tprintf(TEXT("Game not started yet! Try again later.\n"));
			return;
		}
	}

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,
		&dwMode,
		NULL,
		NULL);

	if (!fSuccess) {
		_tprintf(TEXT("SetNamedPipeHandleState falhou. Erro = %d\n"), GetLastError());
		finishAdminClient();
		return;
	}

	startMessaging();

}

void AdminClient::startMessaging()
{

	ReadReady = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (ReadReady == NULL) {
		finishAdminClient();
		return;
	}

	WriteReady = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (WriteReady == NULL) {
		finishAdminClient();
		return;
	}

	vector<string> command;


	_tprintf(TEXT("Administartor connected\n\n"));
	_tprintf(TEXT("Commands avaiable:\n   - objeto <type (2,11)> <number (1,20)>\n   - sair\n\n"));
	while (1) {
		_tprintf(TEXT("Command:  "));
		command = getCommand();

		if (command[0] == "SAIR")
			break;

		if (commandParser(command)) {
			treatCommand(command);


			ZeroMemory(&OverlWr, sizeof(OverlWr));
			ResetEvent(WriteReady);
			OverlWr.hEvent = WriteReady;

			// Escreve no pipe
			fSuccess = WriteFile(
				hPipe,
				&msg,
				msg_sz,
				&cbWritten,
				&OverlWr);

			WaitForSingleObject(WriteReady, INFINITE);


			GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);

			// Le do pipe
			ZeroMemory(&OverlRd, sizeof(OverlRd));
			OverlRd.hEvent = ReadReady;
			ResetEvent(ReadReady);

			fSuccess = ReadFile(
				hPipe,
				&msg,
				msg_sz,
				&cbBytesRead,
				&OverlRd);


			GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);

			WaitForSingleObject(ReadReady, INFINITE);
			_tprintf(TEXT("\nRead concluido"));

			if (cbBytesRead < msg_sz)
				_tprintf(TEXT("\nReadFile falhou. Erro = %d"), GetLastError());

			if (msg.code == SUCCESS) {
				tcout << TEXT("SUCCESS") << endl;
			}
			else {
				tcout << msg.msg << endl;
			}
		}
	}

	CloseHandle(WriteReady);
	CloseHandle(ReadReady);
	finishAdminClient();
}

void AdminClient::finishAdminClient()
{
	tcout << TEXT("Admin client finished") << endl;
	CloseHandle(hPipe);
}

vector<string> AdminClient::getCommand()
{
	char buffer[4096];
	string commandString;
	//get command from inputs
	cin.clear();
	cin.getline(buffer, 4096);
	commandString = buffer;
	commandString = commandToUpperCase(commandString);
	//Vectorize command
	vector <string> comand;
	string temp;
	stringstream ss(commandString);
	while (ss >> temp)
		comand.push_back(temp);

	return comand;
}

//Function that validates the command inputed
bool  AdminClient::commandParser(vector<string> command)
{
	if (!command.size())
		return false;

	//commandos
	if (command[0] == "OBJETO") {
		if (stoi(command[1]) >= 2 && stoi(command[1]) <= 11) {
			if (stoi(command[2]) > 0 && stoi(command[1]) <= 20) {
				return true;
			}
			else {
				tcout << TEXT("[ERRO - Comando] O nr de objectos deve ser entre 1 e 20\n\n") << endl;
				return false;
			}
		}
		else {
			tcout << TEXT("[ERRO - Comando] O Tipo de objecto varia entre 2 e 11\n\n") << endl;
			return false;
		}
	}

	_tprintf(TEXT("[ERRO - Comando] Comando não reconhecido!\n\n"));
	return false;
}

//Function that process the requested command
void AdminClient::treatCommand(vector<string> command)
{
	if (command[0] == "OBJETO") {
		msg.code = SEED_OBJECT;
		sprintf(msg.msg, "%d %d", stoi(command[1]), stoi(command[2]));
	}
}

//Function to convert all string characters into upper case version
string AdminClient::commandToUpperCase(string command)
{
	for (unsigned int i = 0; i < command.size(); i++) {
		if (command[i] >= 'a' && command[i] <= 'z') {
			command[i] -= 32;
		}
	}
	return command;
}