#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

#ifdef _WIN32
const char CLS[4] = "cls";
const int RENDEREDLINES = 20;
#endif
#ifdef __linux__
const char CLS[6] = "clear";
const int RENDEREDLINES = 40;
#endif


// :s save
// :sa <filename> save as
// :q quit
// :w <lineNum> <text> write
// :l <number> gets you to line <number>
// :o <filename> open file
// <number> moves you down by <number> of lines
// <number>- moves you up by <number> of lines


const string SFNAME = "unfinishedFile.tesf";
const string SFNAME2 = "temporaryFile.tesf";
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
	system(CLS);
	int x;
	string line;
	vector<string> arr;
	bool flag = true;
	errors error = NoError;
	int buffer = 0;
	// Checking for file existence
	ifstream tfin(SFNAME);
	ofstream tfon;
	if (!tfin.is_open()) {
		tfon.open(SFNAME);
		tfon.close();
	}
	tfin.close();
	tfin.open(SFNAME2);
	if (!tfin) {
		tfon.open(SFNAME2);
		tfon.close();
	}
	tfin.close();

	showContent(1);
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
			       buffer = writeLine(stoi(arr[1]), line.substr(4 + arr[1].length(), line.length() - arr[1].length()));
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
					cout << e.what() << '\n';
					cout << '"' << arr[0] << '"' << '\n';
					for (auto i : arr) {
						cout << i << '\n';
					}

				}
			} else {
				cout << "Unknown option: " << '"' << line << '"' << '\n';
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
	system(CLS);
	return 0;
}

int showContent(int lineNum) {
	system(CLS);
	int counter = 1; // Keeps track which line currently goind to be printed
	int counter2 = 0; // Keeps track of how many lines are already printed
	string line; // Helping variable for stroing lines inside of it
	string lastLine;
	ifstream fin;
	fin.open(SFNAME);
	if (fin.is_open()) {
		if (lineNum < 1) // Preventing negative lines
			lineNum = 1;
			bool flag = false; // It is true if at least one line was printed to the output
			while (getline(fin, line)) {
				if (counter < lineNum) {
					counter++;
					lastLine = line;
					continue;
				}
				if (counter2 == RENDEREDLINES) // after RENDEREDLINES lines are displayed it stops displaying more of them
				break;
				flag = true; // At least 1 line was printed on the screen
				cout << formatNumber(counter) << "|"  << line << '\n'; // Printing line on the screen
				counter2++;
				counter++;
			}
			if (!flag) {
				cout << formatNumber(counter) << '|' << lastLine << '\n';
			}
			for (int i = 0; i < RENDEREDLINES - counter2; i++) {
				cout << "~\n";
			}
			cout << "----------------\n";
		currentLine = counter;
	} else {
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
	openFile(CURFILENAME); // Opening tesf file after creating it again
	showContent(currentLine); // Going to the line user was before saving file
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
	while (getline(ss, linePart, ' ')) {
		output.push_back(linePart);
	}

	return output;
}

