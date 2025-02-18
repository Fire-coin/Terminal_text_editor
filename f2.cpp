#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
const string HFILE = "helpingFile.hsf";

int format(string);
string strip(string);

int main() {
	format("data1.txt");
	return 0;
}

int format(string inputFile) {
	ifstream fin(inputFile); // File from which program will read
	// filesystem paths
	fs::path helpFile(HFILE); // Path to helping file
	fs::path readFile(inputFile); // Path to input file

	if (fs::exists(helpFile)) { // Removes file if it already exists
		remove(helpFile);
	}
	ofstream fon(HFILE); // Opening helping file
	string line; // Buffer to read from inputFile
	int intent = 0; // Intent on current line
	int lastIntent = 0; // Intent on last line
	bool back = false; // Indicates if escape caracter with backslash is present
	bool inStr = false; // True if currently inside of a string
	char openedWith = NULL; // Stores whether string was opened with ' or "
	int curLine = 0; // Current line
	int changed = 0; // Last index in vector intents
	vector<pair<int, int>> intents = {pair(0, 0)};
	bool significant = false; // Is true 

	if (fin.is_open() && fon.is_open()) {
		while (getline(fin, line)) {
			curLine++;
			for (char i : line) {
				line = strip(line);
				if (i == '\\') {
					back = !back;
					continue;
				}
				if (i == '\'' || i == '"') {
					if (inStr) {
						inStr = false;
						continue;
					}
					if (!inStr) {
						openedWith = i;
						inStr = true;
						continue;
					}
					if (back) {
						back = false;
						continue;
					}
					if (openedWith == i) {
						openedWith = NULL;
						inStr = true;
						continue;
					}
				}
				if (inStr)
					continue;
				if (i == '{') {
					intent++;
					continue;
				}
				if (i == '}' && intent > 0) {
					intent--;
					continue;
				}
			}
			// Generates intents
			string lineNow(min(intent, lastIntent), '\t');
			lineNow += line;
			fon << lineNow << '\n';
			lastIntent = intent;
			
			// Used for debuging
			if (intent != intents[changed].second) {
				intents.push_back(pair(curLine, intent));
				changed++;
			}
			cout << intent << intents[changed].second << '\n';
		}
		// Used for debuging
		for (auto i : intents) {
			cout << i.first << ' ' << i.second << '\n';
		}
	} else {
		cout << "Could not open file\n";
	}
	fin.close();
	fon.close();
	
	fs::remove(readFile);
	fs::copy_file(helpFile, readFile);
	fs::remove(helpFile);

	return 0;
}


string strip(string str) {
	int index = 0;
	while (str[index] == ' ') {
		index++;
	}
	return str.substr(index, str.length() - index);
}