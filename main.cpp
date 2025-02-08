#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
using namespace std;
namespace fs = std::filesystem;
// :s save
// :sa <filename> save as
// :q quit
// :w <lineNum> <text> write
// :l <number> gets you to line <number>
// :o <filename> open file
// <number> moves you down by <number> of lines
// <number>- moves you up by <number> of lines


const string SFNAME = "<unfinishedFile>.txt";
const string SFNAME2 = "<temporaryFile>.txt";
string CURFILENAME = "";
int currentLine = 1;

int saveAsFile(string filename); // done
int openFile(string filename);
int saveFile(); // done
int quit();
int moveToLine(int line);
int writeLine(int lineNum, string line);
int showContent(int lineNum); //done
int moveBy(int lines, bool up);
string formatNumber(int lineNum); // done
vector<string> split(string line);


enum errors {NoError, InsufficientArguments, FileOpening, InvalidLineNumber, NoFilename};

int main() {
	system("clear");
	int x;
	string line;
	//cin >> x;
	//showContent(x);
	//getline(cin, line);
	//getline(cin, line);
	//int success = saveAsFile(line);
	//cout << success << " " << line << '\n';
	vector<string> arr;
	bool flag = true;
	errors error = NoError;
	int buffer = 0;
	while (flag) {
		getline(cin, line);
		arr = split(line);
		buffer = 0;
		if (arr[0] == ":o") {
		       try {
			       buffer = openFile(arr[1]);
		       } catch (const exception& e) {
			       error = InsufficientArguments;
		       }
		} else if  (arr[0] == ":sa") {
		       try {
			       buffer = saveAsFile(arr[1]);
		       } catch (const exception& e) {
			       error = InsufficientArguments;
		       }
		} else if  (arr[0] == ":s") {
		       buffer = saveFile();
		} else if  (arr[0] == ":q") {
		       flag = false;
		} else if (arr[0] == ":w") {
		       try {
			       buffer = writeLine(stoi(arr[1]), line.substr(4 + arr[1].length() + 1, line.length() - arr[1].length()));
			       //buffer = writeLine(stoi(arr[0]), 
		       } catch (const exception& e) {
			       error = InsufficientArguments;
		       }
		} else if (arr[0] == ":l") {
			buffer = showContent(stoi(arr[1]));
		} else {
			if (arr.size() == 1) {
				try {
					int lines = stoi(arr[0]);
					buffer = showContent(currentLine + lines);
				} catch (exception& e) {
					cout << "Unknow option: " << line << '\n';
				}
			} else {
				cout << "Unknown option: " << line << '\n';
			}
		}
		// Checking if there was error
		showContent(currentLine);
		switch (buffer) {
			case 0:
				error = NoError;
				break;
			case -1:
				error = InvalidLineNumber;
				cout << "Invalid line number\n";
				break;
			case -2:
				error = FileOpening;
				cout << "Could not open file\n";
				break;
			case -3:
				error = NoFilename;
				cout << "No filename is currently set\n";
				break;
			default:
				cout << "Unknown error code: " << buffer << '\n';
		}

	}
	ofstream end(SFNAME);
	end << '\n';
	end.close();
	system("clear");
	return 0;
}

int showContent(int lineNum) {
	system("clear");
	int counter = 1; // Keeps track which line currently goind to be printed
	int counter2 = 0; // Keeps track of how many lines are already printed
	string line; // Helping variable for stroing lines inside of it
	string lastLine;
	ifstream fin;
	fin.open(SFNAME);
	if (fin.is_open()) {
		currentLine = lineNum;
		bool flag = false; // It is true if at least one line was printed to the output
		while (getline(fin, line)) {
			if (counter < lineNum) {
				counter++;
				lastLine = line;
				continue;
			}
			if (counter2 == 40) // after 40 lines are displayed it stops displaying more of them
				break;
			flag = true; // At least 1 line was printed on the screen
			cout << formatNumber(counter) << "|--"  << line << '\n'; // Printing line on the screen
			counter2++;
			counter++;
		}
		if (!flag) {
			cout << lastLine << '\n';
		}
		for (int i = 0; i < 40 - counter2; i++) {
			cout << "~\n";
		}
		cout << "----------------\n";
	} else {
		//cout << "Could not open file while showing content\n";
		return -2; // Could not open file
	}
	fin.close();
	return 0;
}

int writeLine(int lineNum, string line) {
	if (lineNum < 1)
		return -1; // Invalid line number
	ofstream fon(SFNAME2);
	ifstream fin(SFNAME);
	if (fon.is_open() && fin.is_open()) {
		int counter = 1;
		string l;
		while (getline(fin, l)) {
			if (counter == lineNum)
				fon << line << '\n';
			else
				fon << l << '\n';
			counter++;
		}
		if (counter <= lineNum) {
			while (counter < lineNum) {
				fon << '\n';
				counter++;
			}
			fon << line << '\n';
		}
	} else {
		return -2; // Could not open file
	}
	fin.close();
	fon.close();

	// rewriting new file content back to SFNAME
	
	fon.open(SFNAME);
	fin.open(SFNAME2);
	
	if (fin.is_open() && fon.is_open()) {
		string l;
		while(getline(fin, l)) {
			fon << l << '\n';
		}
	} else {
		return -2; // Could not open file
	}

	fin.close();
	fon.close();

	return showContent(lineNum);
}

string formatNumber(int lineNumber) {
	string helping = to_string(lineNumber);
	string output;
	if (helping.length() >= 4) {
		return helping;
	}
	output.append(4 - helping.length(), '0');
	output += helping;
	return output;
}

int openFile(string filename) {
	ifstream fin(filename);
	ofstream fon;
	if (!fin) { // Checking if file exists, if no then it creates it
		fon.open(filename);
		fon.close();
	}
	fin.close();
	fin.open(filename);
	fon.open(SFNAME);
	string line;
	CURFILENAME = filename;
	while (getline(fin, line)) {
		fon << line << '\n';
	}
	fon.close();
	fin.close();
	return showContent(1);
}

int saveFile() {
	if (CURFILENAME == "")
		return -3; // No filename is set
	fs::rename(SFNAME, CURFILENAME);	
	ofstream fon(SFNAME); // creating helping file again
	fon.close(); // closing helping file
	openFile(CURFILENAME);
	return 0;
}

int saveAsFile(string filename) {
	CURFILENAME = filename; // Changing filename under which file will be saved
	int buffer = saveFile();
	if (buffer == 0)
		openFile(CURFILENAME);
	return buffer; // Saving file
}

vector<string> split(string line) {
	vector<string> output;
	stringstream ss(line);
	string linePart;
	while (getline(ss, linePart)) {
		output.push_back(linePart);
	}

	return output;
}

