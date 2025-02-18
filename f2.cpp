#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
const string HFILE = "helpingFile.hsf";

//vector<string> split(string line, char delimiter= ' ');
int format(string);

int main() {
	format("data1.txt");
	return 0;
}

int format(string inputFile) {
	ifstream fin(inputFile);
	if (fs::exists(fs::path(HFILE))) {
		remove(fs::path(HFILE));
	}
	ofstream fon(HFILE);
	string line;
	int intent = 0;
	bool back = false;
	bool inStr = false;
	char openedWith = NULL;
	int curLine = 0;
	int changed = 0;
	vector<pair<int, int>> intents = {pair(0, 0)};
	if (fin.is_open() && fon.is_open()) {
		while (getline(fin, line)) {
			curLine++;
			string lineNow(intent * 4, ' ');
			lineNow += line;
			fon << lineNow << '\n';
			for (char i : line) {
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
					//intents.push_back(pair(curLine, intent));
					continue;
				}
				if (i == '}' && intent > 0) {
					intent--;
					//intents.push_back(pair(curLine, intent));
					continue;
				}
			}
			if (intent != intents[changed].second) {
				intents.push_back(pair(curLine, intent));
				changed++;
			}
			cout << intent << intents[changed].second << '\n';
		}
		for (auto i : intents) {
			cout << i.first << ' ' << i.second << '\n';
		}
	} else {
		cout << "Could not open file\n";
	}
	fin.close();
	fon.close();

	return 0;
}
