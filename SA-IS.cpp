#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include<Windows.h>
#include <Psapi.h> 
#include<boost/dynamic_bitset>
using namespace std;

int N = 0;

void fill_array(int* array, int n, int number)
{
	for (int i = 0; i < n; i++) {
		array[i] = number;
	}
}
void print_memory_usage()
{
	//get the handle to this process
	auto myHandle = GetCurrentProcess();
	//to fill in the process' memory usage details
	PROCESS_MEMORY_COUNTERS pmc;
	//return the usage (bytes), if I may
	if (GetProcessMemoryInfo(myHandle, &pmc, sizeof(pmc)))
	{
		auto size = pmc.PeakWorkingSetSize;
		cout <<"Peak set:" << size << endl;
	}
	else
		return;
}

void isprintajPolje(int* polje, int n) {
	int i = 0;
	for (i; i < n; i++) {
		printf("%d ", polje[i]);
	}
	printf("\n");
}

void classify_characters(int level, int len, int* sa, int* t) {
	int offset = N / level;
	for (int i = offset + len - 1; i >= offset; i--) {
		if (i == offset + len - 1) {
			t[i - offset] = 1;
			continue;
		}
		if (sa[i] < sa[i + 1] || sa[i] == sa[i + 1] && t[i - offset + 1]) {
			t[i - offset] = 1;

		}
		else {
			t[i - offset] = 0;
		}
	}
}


int find_lms_substrings(int level, int len, int* sa, int* t, int* p1) {
	int offset = N / level;
	int counter = 0;
	for (int i = offset + 1; i < offset + len; i++) {
		if ((t[i - offset] && !t[i - 1 - offset])) {
			p1[counter++] = i - offset;
		}
	}
	return counter;
}

void move_bk_pointers_to_end(int* mapa, int* b, int num) {
	int counter = 0;
	for (int i = 0; i < num; i++) {
		if (mapa[num] != 0) {
			b[i] = counter + mapa[i] - 1;
			counter += mapa[i];
		}
	}
}

void move_bk_pointers_to_start(int* mapa, int* b, int num) {
	int counter = 0;
	for (int i = 0; i < num; i++) {
		if (mapa[num] != 0) {
			b[i] = counter;
			counter += mapa[i];
		}
	}
}

void induced_sort_lms_substring_substep2(int level, int len, int* sa, int* t, int* b, int* lcp)
{
	int offset = N / level;
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

void induced_sort_lms_substring_substep3(int level, int len, int* sa, int* t, int* b, int* lcp)
{
	int offset = N / level;
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

void induced_sort_lms_substrings(int level, int len, int num_of_characters, int* sa, int* p1, int* t, int* lcp) {
	int* map;
	int* b;
	if (level == 2) {
		num_of_characters = 256;
	}

	map = (int*)malloc(num_of_characters * sizeof(int));
	b = (int*)malloc(num_of_characters * sizeof(int));
	fill_array(map, num_of_characters, 0);

	int offset = N / level;
	for (int i = offset; i < offset + len; i++) {
		map[sa[i]]++;
	}

	for (int i = 0; i < len; i++) {
		sa[i] = -1;
	}

	move_bk_pointers_to_end(map, b, num_of_characters);

	int counter = 0;

	for (int i = offset; i < offset + len; i++) {
		if (p1[counter] == i - offset) {
			sa[b[sa[i]]] = i - offset;
			b[sa[i]]--;
			counter++;
		}
	}
	move_bk_pointers_to_start(map, b, num_of_characters);
	induced_sort_lms_substring_substep2(level, len, sa, t, b, lcp);

	move_bk_pointers_to_end(map, b, num_of_characters);
	induced_sort_lms_substring_substep3(level, len, sa, t, b, lcp);
	free(map);
	free(b);
}

int find_lcp_value(int sa_index_first, int sa_index_second, int* sa) {
	int first = sa[sa_index_first];
	int second = sa[sa_index_second];
	int offset = N / 2;
	int counter = 0;
	if (first == -1 || second == -1) {
		return 0;
	}
	for (int i = 0; first + i + offset < N && second + i + offset < N; i++) {
		if (sa[first + i + offset] != sa[second + i + offset]) break;
		counter++;
	}
	return counter;
}

int rmq(int start, int end, int* lcp) {

	int minimum = N;
	for (int i = start; i <= end; i++) {
		if (lcp[i] == -1)
			continue;
		if (lcp[i] < minimum) {
			minimum = lcp[i];
		}
	}
	if (N == minimum)
		return 0;
	return minimum;
}

void induced_sort_lms_substrings_top(int level, int len, int p2_size, int* sa, int* p2, int* t1, int* lcp, int num_of_characters) {

	int* last_occ;
	int* map;
	int* b;

	map = (int*)malloc(num_of_characters * sizeof(int));
	b = (int*)malloc(num_of_characters * sizeof(int));
	last_occ = (int*)malloc(num_of_characters * sizeof(int));
	fill_array(map, num_of_characters, 0);

	int offset = N / level;

	for (int i = offset; i < offset + len; i++) {
		map[sa[i]]++;
	}


	for (int i = p2_size; i < offset; i++) {
		sa[i] = -1;
	}
	move_bk_pointers_to_end(map, b, num_of_characters);

	for (int i = p2_size - 1; i >= 0; i--) {
		int index = p2[sa[i]];
		int sa_index = b[sa[index + offset]];
		sa[i] = -1;
		sa[sa_index] = index;
		if (level == 2) {
			lcp[sa_index] = 0;
			lcp[sa_index + 1] = find_lcp_value(sa_index, sa_index + 1, sa);
		}
		b[sa[index + offset]]--;
	}
	move_bk_pointers_to_start(map, b, num_of_characters);
	int last = -1;
	//kopirano iz originalne implementacije
	int stack_size = 2 * (512 + num_of_characters + 4);
	int* min_stack = (int*)malloc((stack_size + 4) * sizeof(int));
	min_stack[0] = -1;
	min_stack[1] = -1;
	int stack_end = 1;
	if (level == 2)
	{
		for (int i = 0; i < num_of_characters; i++) {
			last_occ[i] = -1;
		}
		//napuniti last occ sa -1 za svaki znak.
		/*for (auto const& x : b)
		{
			last_occ[x.first] = -1;
		}*/
	}
	for (int i = 0; i < len; i++) {
		if (sa[i] > 0) {

			//Ovo je LS seam
			if (level == 2 && last != -1 && t1[sa[i]] && sa[sa[i] + offset] == sa[sa[last] + offset]) {
				lcp[i] = find_lcp_value(last, i, sa);
				last = -1;
			}
			else if (level == 2 && !t1[sa[i]]) {
				last = i;
			}
			int lcp_val = lcp[i];

			if (!t1[sa[i] - 1]) {
				int k = sa[offset + sa[i] - 1];
				sa[b[k]] = sa[i] - 1;
				if (level == 2) {
					
					//Potrebno napuniti
					if (b[k] == 0) {
						lcp[b[k]] = 0;
						continue;
					}
					//L/S seam
					/*if (b[k] + 1 < offset && sa[sa[b[k]] + offset] == sa[sa[b[k] + 1] + offset] && sa[b[k] + 1] != -1)
					{
						lcp[b[k] + 1] = find_lcp_value(b[k], b[k]+1, sa);
					}*/
					while (lcp_val <= min_stack[stack_end]) {
						stack_end -= 2;
					}; // pop from stack
					min_stack[++stack_end] = i;
					min_stack[++stack_end] = lcp_val;

					if (sa[b[k] - 1] == -1 || sa[sa[b[k]] + offset] != sa[sa[b[k] - 1] + offset]) {
						lcp[b[k]] = 0;
						last_occ[k] = i;
					}
					else {
						int iteration = last_occ[k];
						/*for (int j = 0; j < i; j++) {
							if (sa[j] == sa[b[k] - 1] + 1) {
								iteration = j;
								break;
							}
						}*/

						if (sa[offset + sa[iteration]] != sa[offset + sa[i]]) {
							lcp[b[k]] = 1;
							last_occ[k] = i;
						}
						else {
							//printf("racunam RMQ");
							if (sa[i] == 44) { 
								int h = 0; }
							int start = iteration + 1; // start of query
							int end = stack_end - 3;
							while (start <= min_stack[end]) end -= 2; // search until smaller element found
										// store 0 at bucket beginnings
							lcp[b[k]] = min_stack[end + 3] + 1;      // induce LCP-value!
							last_occ[k] = i;
							//lcp[b[k]] = rmq(iteration + 1, i, lcp) + 1;
						}
					}
				}
				b[k]++;
			}
		}
		// don't induce, but update stack with LCP[i]
		int lcp_val = lcp[i];      // get current LCP-value
		if (lcp_val >= 0) {    // check if already computed
			while (lcp_val <= min_stack[stack_end]) {
				stack_end -= 2;
			}; // pop from stack
			min_stack[++stack_end] = i;
			min_stack[++stack_end] = lcp_val;
			
		}
		
		

		if (stack_end > stack_size) {
			min_stack = (int*)realloc(min_stack, (stack_size * 2 + 4) * sizeof(int));
			stack_size *= 2 + 2;
		}
	}
	
	move_bk_pointers_to_end(map, b, num_of_characters);
	//ne bi diral ls seam jer bi ovde trebalo ici po redu. ak se nekaj preskoci, ne bu napisano vise...
	//printf("zadnja for petlja\n");
	stack_end = 1;
	min_stack[0] = offset; min_stack[1] = -1;
	if (level == 2)
	{
		//napuniti last occ sa -1 za svaki znak.
		for (int i = 0; i < num_of_characters; i++) {
			last_occ[i] = offset - 1;
		}
		/*for (auto const& x : b)
		{
			last_occ[x.first] = offset - 1;
		}*/
	}
	for (int i = len - 1; i >= 0; i--) {
		if (sa[i] > 0) {
			if (t1[sa[i] - 1]) {
			
				int k = sa[sa[i] - 1 + offset];
				sa[b[k]] = sa[i] - 1;
				if (level == 2) {
					//printf("racunam S sufix");
					if (b[k] + 1 == N) {
						last_occ[k] = i;
						//ovo treba razmotriti
					}
					else if (sa[b[k] + 1] == -1 || sa[sa[b[k]] + offset] != sa[sa[b[k] + 1] + offset]) {
						//L/S seam
						if (b[k] - 1 > 0 && sa[sa[b[k]] + offset] == sa[sa[b[k] - 1] + offset] && !t1[sa[b[k] - 1]])
						{
							lcp[b[k]] = find_lcp_value(b[k] - 1, b[k], sa);
						}
						last_occ[k] = i;

					}
					else {
						//L/S seam
						if (b[k] - 1 > 0 && sa[sa[b[k]] + offset] == sa[sa[b[k] - 1] + offset] && !t1[sa[b[k] - 1]])
						{
							lcp[b[k]] = find_lcp_value(b[k] - 1, b[k], sa);
						}

						int iteration = last_occ[k];
						/*for (int j = len - 1; j > i; j--) {
							if (sa[j] == sa[b[k] + 1] + 1) {
								iteration = j;
								break;
							}
						}*/

						if (sa[offset + sa[iteration]] != sa[offset + sa[i]]) {
							lcp[b[k] + 1] = 1;
							last_occ[k] = i;
						}
						else {
							int start = iteration;     // end of query
							int end = stack_end - 1;
							while (start >= min_stack[end]) end -= 2; // search until smaller element found
							lcp[b[k] + 1] = min_stack[end + 3] + 1;    // induce LCP-value!

							last_occ[k] = i;
							//lcp[b[k] + 1] = rmq(i + 1, iteration, lcp) + 1;
						}
					}
					

				}

				b[k]--;
			}
			// update MinStack:
			int lcp_val = lcp[i];
			while (lcp_val <= min_stack[stack_end]) {
				stack_end -= 2;
			}// pop from stack
			min_stack[++stack_end] = i;
			min_stack[++stack_end] = lcp_val;
		}
	}
	free(min_stack);
	free(map);
	free(b);
	free(last_occ);
}


bool compare(int level, int* sa, int* t, int last, int index) {
	if (last == -1)
		return false;
	bool same = true;
	int offset = N / level;
	int new_size = N / (level / 2);
	for (int i = last, j = index; i < new_size && j < new_size;) {
		if (sa[i + offset] != sa[j + offset] || t[i] != t[j]) {
			same = false;
			break;
		}
		//ako je neki od trenutnih znakova LMS prekidamo
		if (i != last && ((i > 0 && t[i] && !t[i-1]) || (j > 0 && t[j] && !t[j-1]))) break;

		i++;
		j++;
	}
	return same;
}

pair<bool, int> lms_substring_naming(int level, int len, int p1_size, int* sa, int* t, int* p1) {
	int offset = N / level;
	int last = -1;
	/*for (int i = 0; i < p1.size(); i++) {
		s1.push_back(-1);
	}*/
	int counter = 0;
	for (int i = 0; i < len; i++) {
		if (sa[i] > 0 && t[sa[i]] && !t[sa[i] - 1]) {
			sa[counter++] = sa[i];
		}
	}
	//oslobodi drugu polovicu polja
	for (int i = offset / 2; i < offset; i++)
		sa[i] = -1;
	bool unique = true;
	int name_counter = 0;
	for (int i = 0; i < p1_size; i++) {
		int index = sa[i];
		if (index > 0 && t[index] && !t[index - 1]) {
			if (compare(level, sa, t, last, index)) {

				unique = false;
				sa[index / 2 + offset / 2] = name_counter - 1;
				last = index;
			}
			else {
				sa[index / 2 + offset / 2] = name_counter++;
				last = index;
			}
		}
	}
	//vrati natrag imena
	int last_empty = offset / 2;
	for (int i = offset / 2; i < offset; i++) {
		if (sa[i] != -1) {
			sa[last_empty] = sa[i];
			last_empty++;
		}
	}

	return make_pair(unique, name_counter);
}



int sa_is(int level, int len, int* sa, int* lcp, int num_of_characters) {
	int* t;
	int* p1;

	t = (int*)malloc((N /level) * sizeof(int));
	p1 = (int*)malloc((len / 2) * sizeof(int));

	int offset = N / level;
	classify_characters(level, len, sa, t);
	int p1_size = find_lms_substrings(level, len, sa, t, p1);
	induced_sort_lms_substrings(level, len, num_of_characters, sa, p1, t, lcp);
	pair<bool, int> unique = lms_substring_naming(level, len, p1_size, sa, t, p1);

	if (unique.first) {
		//fill_sa(sa1, s1.size());
		for (int i = 0; i < p1_size; i++) {
			sa[sa[i + offset / 2]] = i;
		}

	}
	else {
		fill_array(sa, offset / 2, -1);
		sa_is(2 * level, p1_size, sa, lcp, unique.second);
	}
	induced_sort_lms_substrings_top(level, len, p1_size, sa, p1, t, lcp, num_of_characters);
	free(t);
	free(p1);
	return 0;
}


int main(int argc, const char *argv[]) {
	
	std::ifstream inFile;
	inFile.open("C:/nn/Eccherichia_coli_no_spaces.txt"); //open the input file
	//inFile.open("C:/nn/file.exe");
	std::stringstream strStream;
	strStream << inFile.rdbuf(); //read the file
	inFile.close();
	//string temp = "ATTAGCGAGCG$";
	string temp = strStream.str();
	//string temp = "cabacbbabacbbc$";
	//string temp = "ABANANABANDANA$";
	//string temp = "banana$";
	int n = temp.length();
	N = temp.length() * 2;
	int* sa = (int*)malloc(n * sizeof(int) * 2);
	int* lcp = (int*)malloc(n * sizeof(int));

	for (int i = 0; i < n; i++) {
		sa[i] = -1;
		lcp[i] = -1;
		//sa.push_back(-1);
		//lcp.push_back(-1);
	}
	for (int i = 0; i < n; i++) {
		sa[i + n] = temp[i];
		//sa.push_back(temp[i]);
	}
	
	print_memory_usage();

	printf("Starting.\n");
	clock_t start = clock();
	sa_is(2, temp.length(), sa, lcp, 256);

	clock_t finish = clock();
	cout << "Duration time: " << (double)(finish - start) / (double)CLOCKS_PER_SEC;
	print_memory_usage();
	FILE* sa_file;
	FILE* lcp_file;
	fopen_s(&sa_file, "C:/nn/saN.txt", "w+");
	fopen_s(&lcp_file, "C:/nn/lcpN.txt", "w+");
	for (int i = 0; i < n; i++) {
		fprintf(sa_file, "%d\n", sa[i]);
	}

	for (int i = 0; i < n; i++) {
		fprintf(lcp_file, "%d\n", lcp[i]);
	}

	fclose(sa_file);
	fclose(lcp_file);
	/*ofstream saFile, lcpFile;
	saFile.open("C:/nn/sa.txt");
	lcpFile.open("C:/nn/lcp.txt");
	for (int i = 0; i < N / 2; i++)
	{
		saFile << sa[i] << '\n';
		lcpFile << lcp[i] << '\n';
	}
	saFile.close();
	lcpFile.close();
	*/
	free(sa);
	free(lcp);
	int g = 0;
}