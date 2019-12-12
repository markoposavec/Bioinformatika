#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

using namespace std;

void classify_characters(string s, vector<bool> &t) {
	for (int i = s.length() - 1; i >= 0; i--) {
		if (i == s.length() - 1) {
			t.push_back(true);
			continue;
		}
		if (s[i] < s[i + 1] || s[i] == s[i + 1] && t[t.size() - 1]) {
			t.push_back(true);
		}
		else {
			t.push_back(false);
		}
	}

	reverse(t.begin(), t.end());
}

void find_lms_substrings(string s, vector<bool> t, vector<int> &p1) {
	for (int i = 1; i < s.length(); i++) {
		if ((t[i] == true && t[i - 1] == false)) {
			p1.push_back(i);
		}
	}
}

void move_bk_pointers_to_end(map<char, int> mapa, map<char, int> &b) {
	int counter = 0;
	for (auto const& x : mapa) {
		b[x.first] = counter + x.second - 1;
		counter += x.second;
	}
}

void move_bk_pointers_to_start(map<char, int> mapa, map<char, int> &b) {
	int counter = 0;
	for (auto const& x : mapa)
	{
		b[x.first] = counter;
		counter += x.second;
	}
}

void induced_sort_lms_substrings(string s, vector<int> &sa, vector<int> &p1, vector<bool> t) {
	map<char, int> b;
	map<char, int> map;
	for (long i = 0; i < s.length(); i++) {
		map[s[i]]++;
	}

	for (int i = 0; i < s.length(); i++) {
		sa.push_back(-1);
	}
	
	move_bk_pointers_to_end(map, b);

	int counter = 0;
	for (int i = 0; i < s.length(); i++) {
		if (p1[counter] == i) {
			sa[b[s[i]]] = i;
			b[s[i]]--;
			counter++;
		}
	}

	move_bk_pointers_to_start(map, b);

	for (int i = 0; i < sa.size(); i++) {
		if (sa[i] > 0) {
			if (!t[sa[i] - 1]) {
				char k = s[sa[i] - 1];
				sa[b[k]] = sa[i] - 1;
				b[k]++;
			}
		}
	}

	move_bk_pointers_to_end(map, b);

	for (int i = sa.size() - 1; i >= 0; i--) {
		if (sa[i] > 0) {
			if (t[sa[i] - 1]) {
				char k = s[sa[i] - 1];
				sa[b[k]] = sa[i] - 1;
				b[k]--;
			}
		}
	}

}

int sa_is(string s, vector<int> &sa) {
	int n = s.length();
	vector<bool> t;
	vector<int> p1;
	vector<int> s1;
	vector<int> b;
	// možda treba promjeniti
	int numberOfDifferentCharacters = 5;

	classify_characters(s, t);
	find_lms_substrings(s, t, p1);

	induced_sort_lms_substrings(s, sa, p1, t);
	return 0;
}


int main() {
	vector<int> sa;
	sa_is("ATTAGCGAGCG$", sa);
}