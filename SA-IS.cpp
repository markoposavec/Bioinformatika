#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

using namespace std;

void classify_characters(vector<int> &s, vector<bool>& t) {
	for (int i = s.size() - 1; i >= 0; i--) {
		if (i == s.size() - 1) {
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

void find_lms_substrings(vector<int> &s, vector<bool> &t, vector<int>& p1) {
	for (int i = 1; i < s.size(); i++) {
		if ((t[i] == true && t[i - 1] == false)) {
			p1.push_back(i);
		}
	}
}

void move_bk_pointers_to_end(map<int, int> &mapa, map<int, int>& b) {
	int counter = 0;
	for (auto const& x : mapa) {
		b[x.first] = counter + x.second - 1;
		counter += x.second;
	}
}

void move_bk_pointers_to_start(map<int, int> &mapa, map<int, int>& b) {
	int counter = 0;
	for (auto const& x : mapa)
	{
		b[x.first] = counter;
		counter += x.second;
	}
}

void induced_sort_lms_substring_substep2(vector<int> &s, vector<int>& sa, vector<bool> &t, map<int, int> &b)
{
	for (int i = 0; i < sa.size(); i++) {
		if (sa[i] > 0) {
			if (!t[sa[i] - 1]) {
				int k = s[sa[i] - 1];
				sa[b[k]] = sa[i] - 1;
				b[k]++;
			}
		}
	}

}

void induced_sort_lms_substring_substep3(vector<int> &s, vector<int>& sa, vector<bool> &t, map<int, int> &b)
{
	for (int i = sa.size() - 1; i >= 0; i--) {
		if (sa[i] > 0) {
			if (t[sa[i] - 1]) {
				int k = s[sa[i] - 1];
				sa[b[k]] = sa[i] - 1;
				b[k]--;
			}
		}
	}

}

void induced_sort_lms_substrings(vector<int> &s, vector<int>& sa, vector<int>& p1, vector<bool> &t) {
	map<int, int> b;
	map<int, int> map;
	for (long i = 0; i < s.size(); i++) {
		map[s[i]]++;
	}

	for (int i = 0; i < sa.size(); i++) {
		sa[i] = -1;
	}

	move_bk_pointers_to_end(map, b);

	int counter = 0;
	for (int i = 0; i < s.size(); i++) {
		if (p1[counter] == i) {
			sa[b[s[i]]] = i;
			b[s[i]]--;
			counter++;
		}
	}

	move_bk_pointers_to_start(map, b);
	induced_sort_lms_substring_substep2(s, sa, t, b);

	move_bk_pointers_to_end(map, b);
	induced_sort_lms_substring_substep3(s, sa, t, b);

}

void induced_sort_lms_substrings_top(vector<int> &s, vector<int>& sa1, vector<int> &sa2, vector<int>&p2, vector<bool> &t1) {
	map<int, int> b;
	map<int, int> map;
	for (long i = 0; i < s.size(); i++) {
		map[s[i]]++;
	}

	for (int i = 0; i < sa1.size(); i++) {
		sa1[i] = -1;
	}

	move_bk_pointers_to_end(map, b);

	for (int i = sa2.size() - 1; i >= 0; i--) {
		int index = p2[sa2[i]];
		sa1[b[s[index]]] = index;
		b[s[index]]--;
	}

	move_bk_pointers_to_start(map, b);
	induced_sort_lms_substring_substep2(s, sa1, t1, b);

	move_bk_pointers_to_end(map, b);
	induced_sort_lms_substring_substep3(s, sa1, t1, b);

}


bool compare(vector<int> &s, vector<bool> &t, int last, int index) {
	if (last == -1)
		return false;
	bool same = true;
	for (int i = last, j = index; i < s.size() && j < s.size();) {
		if (s[i] != s[j] || t[i] != t[j]) {
			same = false;
			break;
		}
		if (t[i] == t[j] && t[i] && i != last) break;

		i++;
		j++;
	}
	return same;
}

pair<vector<int>, bool> lms_substring_naming(vector<int> &s, vector<int> &sa, vector<bool> &t, vector<int> &p1) {
	int last = -1;
	vector<int> s1;
	for (int i = 0; i < p1.size(); i++) {
		s1.push_back(-1);
	}
	bool unique = true;
	int name_counter = 0;
	for (int i = 0; i < sa.size(); i++) {
		int index = sa[i];
		for (int j = 0; j < p1.size(); j++) {
			if (p1[j] == index) {
				if (compare(s, t, last, index)) {
					//potencijalno krivo hehe
					unique = false;
					s1[j] = name_counter - 1;
					last = index;
				}
				else {
					s1[j] = name_counter++;
					last = index;
				}
				break;
			}
		}
	}
	return make_pair(s1, unique);
}

void fill_sa(vector<int>& sa, int n)
{
	for (int i = 0; i < n; i++) {
		sa.push_back(-1);
	}
}

int sa_is(vector<int> &s, vector<int>& sa) {
	int n = s.size();
	vector<bool> t;
	vector<int> p1;
	vector<int> s1;
	vector<int> b;
	// možda treba promjeniti
	int numberOfDifferentCharacters = 5;

	classify_characters(s, t);
	find_lms_substrings(s, t, p1);

	induced_sort_lms_substrings(s, sa, p1, t);

	pair<vector<int>, bool> substring_naming = lms_substring_naming(s, sa, t, p1);
	bool unique = substring_naming.second;
	s1 = substring_naming.first;

	vector<int> sa1;
	if (unique) {
		fill_sa(sa1, s1.size());
		for (int i = 0; i < s1.size(); i++) {
			sa1[s1[i]] = i;
		}
	}
	else {
		fill_sa(sa1, s1.size());
		sa_is(s1, sa1);
		

	}
	induced_sort_lms_substrings_top(s, sa, sa1, p1, t);
	return 0;
}


int main() {
	vector<int> s;
	vector<int> sa;
	string temp = "ATTAGCGAGCG$";
	for (int i = 0; i < temp.length(); i++) {
		s.push_back(temp[i]);
	}
	fill_sa(sa, s.size());
	sa_is(s, sa);
	int g = 0;
}