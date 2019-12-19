#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

using namespace std;

void classify_characters(int level, int len, vector<int> &sa, vector<bool>& t) {
	int offset = sa.size() / level;
	for (int i = offset + len - 1; i >= offset; i--) {
		if (i == offset + len - 1) {
			t.push_back(true);
			continue;
		}
		if (sa[i] < sa[i + 1] || sa[i] == sa[i + 1] && t[t.size() - 1]) {
			t.push_back(true);
		}
		else {
			t.push_back(false);
		}
	}

	reverse(t.begin(), t.end());
}

void find_lms_substrings(int level, int len, vector<int> &sa, vector<bool> &t, vector<int>& p1) {
	int offset = sa.size() / level;

	for (int i = offset + 1; i < offset + len; i++) {
		if ((t[i - offset] == true && t[i - 1 - offset] == false)) {
			p1.push_back(i - offset);
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

void induced_sort_lms_substring_substep2(int level, int len, vector<int>& sa, vector<bool> &t, map<int, int> &b)
{
	int offset = sa.size() / level;
	for (int i = 0; i < len; i++) {
		if (sa[i] > 0) {
			if (!t[sa[i] - 1]) {
				int k = sa[offset + sa[i] - 1];
				sa[b[k]] = sa[i] - 1;
				b[k]++;
			}
		}
	}

}

void induced_sort_lms_substring_substep3(int level, int len, vector<int>& sa, vector<bool> &t, map<int, int> &b)
{
	int offset = sa.size() / level;
	for (int i = len - 1; i >= 0; i--) {
		if (sa[i] > 0) {
			if (t[sa[i] - 1]) {
				int k = sa[sa[i] - 1 + offset];
				sa[b[k]] = sa[i] - 1;
				b[k]--;
			}
		}
	}

}

void induced_sort_lms_substrings(int level, int len, vector<int>& sa, vector<int>& p1, vector<bool> &t) {
	map<int, int> b;
	map<int, int> map;
	int offset = sa.size() / level;
	for (int i = offset; i < offset + len; i++) {
		map[sa[i]]++;
	}

	for (int i = 0; i < len; i++) {
		sa[i] = -1;
	}

	move_bk_pointers_to_end(map, b);

	int counter = 0;
	for (int i = offset; i < offset + len; i++) {
		if (p1[counter] == i - offset) {
			sa[b[sa[i]]] = i - offset;
			b[sa[i]]--;
			counter++;
		}
	}

	move_bk_pointers_to_start(map, b);
	induced_sort_lms_substring_substep2(level, len, sa, t, b);

	move_bk_pointers_to_end(map, b);
	induced_sort_lms_substring_substep3(level, len, sa, t, b);

}

void induced_sort_lms_substrings_top(int level, int len, vector<int> &sa, vector<int>&p2, vector<bool> &t1) {
	map<int, int> b;
	map<int, int> map;
	int offset = sa.size() / level;

	for (int i = offset; i < offset + len; i++) {
		map[sa[i]]++;
	}

	for (int i = 0; i < len; i++) {
		sa[i + offset] = -1;
	}
	move_bk_pointers_to_end(map, b);

	for (int i = len - 1; i >= 0; i--) {
		int index = p2[sa[i]];
		sa[b[sa[index]]] = index;
		b[sa[index]]--;
	}

	move_bk_pointers_to_start(map, b);
	induced_sort_lms_substring_substep2(level, len, sa, t1, b);

	move_bk_pointers_to_end(map, b);
	induced_sort_lms_substring_substep3(level, len, sa, t1, b);

}


bool compare(int level, vector<int> &sa, vector<bool> &t, int last, int index) {
	if (last == -1)
		return false;
	bool same = true;
	int offset = sa.size() / level;
	for (int i = last, j = index; i < sa.size() / (level / 2) && j < sa.size() / (level / 2);) {
		if (sa[i + offset] != sa[j + offset] || t[i] != t[j]) {
			same = false;
			break;
		}
		if (t[i] == t[j] && t[i] && i != last) break;

		i++;
		j++;
	}
	return same;
}

bool lms_substring_naming(int level, int len, vector<int> &sa, vector<bool> &t, vector<int> &p1) {
	int offset = sa.size() / level;
	int last = -1;
	/*for (int i = 0; i < p1.size(); i++) {
		s1.push_back(-1);
	}*/
	int counter = 0;
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < p1.size(); j++) {
			if (sa[i] == p1[j])
				sa[counter++] = sa[i];
		}
	}
	bool unique = true;
	int name_counter = 0;
	for (int i = 0; i < p1.size(); i++) {
		int index = sa[i];
		for (int j = 0; j < p1.size(); j++) {
			//Ako je zadnji onda izađi
			if (p1[j] == index) {
				if (compare(level, sa, t, last, index)) {
					//potencijalno krivo hehe
					unique = false;
					sa[j + offset / 2] = name_counter - 1;
					last = index;
				}
				else {
					sa[j + offset / 2] = name_counter++;
					last = index;
				}
				break;
			}
		}
		
	}
	return unique;
}

void fill_sa(vector<int>& sa, int n)
{
	for (int i = 0; i < n; i++) {
		sa[i] = -1;
	}
}

int sa_is(int level, int len, vector<int>& sa) {
	int n = sa.size();
	vector<bool> t;
	vector<int> s1;
	vector<int> p1;
	vector<int> b;
	int offset = sa.size() / level;
	classify_characters(level, len, sa, t);
	find_lms_substrings(level, len, sa, t, p1);

	induced_sort_lms_substrings(level, len, sa, p1, t);

	bool unique = lms_substring_naming(level, len, sa, t, p1);
	
	if (unique) {
		//fill_sa(sa1, s1.size());
		for (int i = 0; i < p1.size(); i++) {
			sa[sa[i + offset / 2]] = i;
		}
	}
	else {
		fill_sa(sa, offset / 2);
		sa_is(2 * level, p1.size(), sa);
	}
	induced_sort_lms_substrings_top(level, len, sa, p1, t);
	return 0;
}


int main() {
	vector<int> sa;
	string temp = "ATTAGCGAGCG$";
	for (int i = 0; i < temp.length(); i++) {
		sa.push_back(-1);
	}
	for (int i = 0; i < temp.length(); i++) {
		sa.push_back(temp[i]);
	}
	sa_is(2, temp.length(), sa);
	int g = 0;
}