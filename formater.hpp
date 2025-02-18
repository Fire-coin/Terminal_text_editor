#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

string strip(string);

vector<pair<int, int>> format(string inputFile, int insertedLine= -1) {
	ifstream fin(inputFile); // File from which program will read

	string line; // Buffer to read from inputFile
	int intent = 0; // Intent on current line
	int lastIntent = 0; // Intent on last line
	bool back = false; // Indicates if escape caracter with backslash is present
	bool inStr = false; // True if currently inside of a string
	// Value of 255 should be treated as NULL
	unsigned char openedWith = 255; // Stores whether string was opened with ' or "
	int curLine = 0; // Current line
	int changed = 0; // Last index in vector intents
	vector<pair<int, int>> intents = {pair(0, 0)};
	bool slash = false;

	if (fin.is_open()) {
		while (getline(fin, line)) {
			curLine++;
			slash = false;
			int counter = 0;
			line = strip(line);
			for (char i : line) {
				counter++;
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
						openedWith = 255;
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

				if (i == '/') {
					if (slash)
						break;
					slash = true;
				}
			}

			if (intent != intents[changed].second) {
				intents.push_back(pair(curLine, min(intent, lastIntent)));
				changed++;
			}
			if (curLine == insertedLine - 1) {
				intents.push_back(pair(insertedLine, intent));
				changed++;
				curLine++;
			}
            lastIntent = intent;
		}
	} else {
		cout << "Could not open file\n";
	}
	fin.close();

	return intents;
}


string strip(string str) {
	int index = 0;
	while (str[index] == ' ' || str[index] == '\t') {
		index++;
	}
	return str.substr(index, str.length() - index);
}