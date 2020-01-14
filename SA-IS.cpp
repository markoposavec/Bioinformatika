#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>

//#include<Windows.h>
//#include <Psapi.h> 
using namespace std;

//Global variable for SA array size. SA array size is two times larger than original text.
int N = 0;

/*
Function for filling array with same number.
*/
void FillArray(int* array, int n, int number)
{
	for (int i = 0; i < n; i++) {
		array[i] = number;
	}
}
//Function for printing WorkingSetSize on Windows. Used for testing. It needs Windows.h and Psapi.h included.
/*void print_memory_usage()
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
}*/

/*Function for printing array of size n.*/
void PrintArray(int* array, int n) {
	int i = 0;
	for (i; i < n; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

/*Function that classifies characters as L or S suffixes. Characters are written in the [offset, len] part of SA array.
Classification is written in array t.
*/
void ClassifyCharacters(int level, int len, int* sa, char* t) {
	int offset = N / level;
	for (int i = offset + len - 1; i >= offset; i--) {
		//last character is always S
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

//Function for finding LMS substrings. LMS substrings are written in p array.
int FindLmsSubstrings(int level, int len, int* sa, char* t, int* p1) {
	int offset = N / level;
	int counter = 0;
	for (int i = offset + 1; i < offset + len; i++) {
		if ((t[i - offset] && !t[i - 1 - offset])) {
			p1[counter++] = i - offset;
		}
	}
	return counter;
}

/*
Map has number of appearances of every character. Size of b is number of buckets. b[i] has current pointer for bucket i.
Function sets pointers to the end of bucket.
*/
void MoveBkPointersToEnd(int* map, int* b, int num) {
	int counter = 0;
	for (int i = 0; i < num; i++) {
		if (map[i] != 0) {
			b[i] = counter + map[i] - 1;
			counter = counter + map[i];
		}
	}
}

/*
Map has number of appearances of every character. Size of b is number of buckets. b[i] has current pointer for bucket i.
Function sets pointers to the start of bucket.
*/
void MoveBkPointersToStart(int* map, int* b, int num) {
	int counter = 0;
	for (int i = 0; i < num; i++) {
		if (map[i] != 0) {
			b[i] = counter;
			counter += map[i];
		}
	}
}

/*
Second substep for inducing SA. L suffixes are written in buckets.
*/
void InducedSortLmsSubstringSubstep2(int level, int len, int* sa, char* t, int* b, int* lcp)
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

/*
Third substep for inducing SA. S suffixes are written in buckets.
*/
void InducedSortLmsSubstringSubstep3(int level, int len, int* sa, char* t, int* b, int* lcp)
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

/*
Function for inducing SA downwards.
*/
void InducedSortLmsSubstrings(int level, int len, int num_of_characters, int* sa, int* p1, char* t, int* lcp) {
	int* map;
	int* b;
	if (level == 2) {
		num_of_characters = 256;
	}

	map = (int*)malloc(num_of_characters * sizeof(int));
	b = (int*)malloc(num_of_characters * sizeof(int));
	FillArray(map, num_of_characters, 0);
	int offset = N / level;
	for (int i = offset; i < offset + len; i++) {
		map[sa[i]]++;
	}
	for (int i = 0; i < len; i++) {
		sa[i] = -1;
	}

	//Substep 1: Placing LMS substrings in buckets.
	MoveBkPointersToEnd(map, b, num_of_characters);
	int counter = 0;

	for (int i = offset; i < offset + len; i++) {
		if (p1[counter] == i - offset) {
			sa[b[sa[i]]] = i - offset;
			b[sa[i]]--;
			counter++;
		}
	}
	//Substep 2
	MoveBkPointersToStart(map, b, num_of_characters);
	InducedSortLmsSubstringSubstep2(level, len, sa, t, b, lcp);
	//Substep 3
	MoveBkPointersToEnd(map, b, num_of_characters);
	InducedSortLmsSubstringSubstep3(level, len, sa, t, b, lcp);
	free(map);
	free(b);
}

/*
Function for naive computing LCP value, by direct comparison.
*/
int FindLcpValue(int sa_index_first, int sa_index_second, int* sa) {
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

/*
Function for naive computing of RMQ, by direct scanning interval.
*/
int Rmq(int start, int end, int* lcp) {

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

/*
Function for inducing SA and LCP upwards. If level is 2 (inducing last SA), LCP is also computed.
*/
void InducedSortLmsSubstringsTop(int level, int len, int p2_size, int* sa, int* p2, char* t1, int* lcp, int num_of_characters) {

	int* last_occ;
	int* map;
	int* b;

	map = (int*)malloc(num_of_characters * sizeof(int));
	b = (int*)malloc(num_of_characters * sizeof(int));
	last_occ = (int*)malloc(num_of_characters * sizeof(int));
	FillArray(map, num_of_characters, 0);

	int offset = N / level;

	for (int i = offset; i < offset + len; i++) {
		map[sa[i]]++;
	}


	for (int i = p2_size; i < offset; i++) {
		sa[i] = -1;
	}
	//Substep 1 for inducing.
	MoveBkPointersToEnd(map, b, num_of_characters);

	for (int i = p2_size - 1; i >= 0; i--) {
		int index = p2[sa[i]];
		int sa_index = b[sa[index + offset]];
		sa[i] = -1;
		sa[sa_index] = index;
		if (level == 2) {
			//Current LCP value is set to 0. Value on the right is directly computed.
			lcp[sa_index] = 0;
			lcp[sa_index + 1] = FindLcpValue(sa_index, sa_index + 1, sa);
		}
		b[sa[index + offset]]--;
	}

	//Substep 2
	MoveBkPointersToStart(map, b, num_of_characters);
	int last = -1;
	//MinStack taken from original Fischer implementation.
	int stack_size = 2 * (512 + num_of_characters + 4);
	int* min_stack = (int*)malloc((stack_size + 4) * sizeof(int));
	min_stack[0] = -1;
	min_stack[1] = -1;
	int stack_end = 1;
	if (level == 2)
	{
		//Last_occ stores index of last appeared suffix in bucket. 
		for (int i = 0; i < num_of_characters; i++) {
			last_occ[i] = -1;
		}
	}

	for (int i = 0; i < len; i++) {
		if (sa[i] > 0) {

			//Check L/S seam. Update LCP value of first S suffix in bucket, considering last L suffix.
			//Last variable stores index of last L suffix
			if (level == 2 && last != -1 && t1[sa[i]] && sa[sa[i] + offset] == sa[sa[last] + offset]) {
				lcp[i] = FindLcpValue(last, i, sa);
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
					//if its $ written, place lcp on 0.
					if (b[k] == 0) {
						lcp[b[k]] = 0;
						continue;
					}
					//Min_stack adjustment. 
					while (lcp_val <= min_stack[stack_end]) {
						stack_end -= 2;
					}; // pop from stack
					min_stack[++stack_end] = i;
					min_stack[++stack_end] = lcp_val;
					//if its first L suffix in bucket, place LCP on 0.
					if (sa[b[k] - 1] == -1 || sa[sa[b[k]] + offset] != sa[sa[b[k] - 1] + offset]) {
						lcp[b[k]] = 0;
						last_occ[k] = i;
					}
					else {
						//find iteration in which left suffix was induced.
						int iteration = last_occ[k];
						//if iteration and i are in different buckets, LCP is 1, else compute RMQ[iteration+1, i].
						if (sa[offset + sa[iteration]] != sa[offset + sa[i]]) {
							lcp[b[k]] = 1;
							last_occ[k] = i;
						}
						else {
							//RMQ query
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


		//Resize stack if its too big.
		if (stack_end > stack_size) {
			min_stack = (int*)realloc(min_stack, (stack_size * 2 + 4) * sizeof(int));
			stack_size *= 2 + 2;
		}
	}

	//Substep 3.
	MoveBkPointersToEnd(map, b, num_of_characters);
	stack_end = 1;
	min_stack[0] = offset; 
	min_stack[1] = -1;
	if (level == 2)
	{
		for (int i = 0; i < num_of_characters; i++) {
			last_occ[i] = offset - 1;
		}
	}

	for (int i = len - 1; i >= 0; i--) {

		if (sa[i] > 0) {
			if (t1[sa[i] - 1]) {

				int k = sa[sa[i] - 1 + offset];
				sa[b[k]] = sa[i] - 1; //induce SA

				if (level == 2) {
					if (b[k] + 1 == N) {
						//if its last, doesnt' matter.
						last_occ[k] = i;
					}
					//check if suffix is written on the end of the bucket.
					else if (sa[b[k] + 1] == -1 || sa[sa[b[k]] + offset] != sa[sa[b[k] + 1] + offset]) {
						//Check if written S suffix is on LS seam. Update naively if it is.
						if (b[k] - 1 > 0 && sa[sa[b[k]] + offset] == sa[sa[b[k] - 1] + offset] && !t1[sa[b[k] - 1]])
						{
							lcp[b[k]] = FindLcpValue(b[k] - 1, b[k], sa);
						}
						last_occ[k] = i;

					}
					else {
						//Check if written S suffix is on LS seam. Update naively if it is.
						if (b[k] - 1 > 0 && sa[sa[b[k]] + offset] == sa[sa[b[k] - 1] + offset] && !t1[sa[b[k] - 1]])
						{
							lcp[b[k]] = FindLcpValue(b[k] - 1, b[k], sa);
						}

						//Get iteration in which last S suffix in bucket was written-
						int iteration = last_occ[k];
						//If iteration and i are in different buckets, LCP is 1, else compute RMQ.
						if (sa[offset + sa[iteration]] != sa[offset + sa[i]]) {
							lcp[b[k] + 1] = 1;
							last_occ[k] = i;
						}
						else {
							//COmpute RMQ.
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
			if (level == 2) {
				// update MinStack:
				int lcp_val = lcp[i];
				while (lcp_val <= min_stack[stack_end]) {
					stack_end -= 2;
				}// pop from stack
				min_stack[++stack_end] = i;
				min_stack[++stack_end] = lcp_val;
			}
		}
	}

	free(min_stack);
	free(map);
	free(b);
	free(last_occ);
}

//Naive comparison of two LMS suffixes.
bool Compare(int level, int* sa, char* t, int last, int index) {
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
		if (i != last && ((i > 0 && t[i] && !t[i - 1]) || (j > 0 && t[j] && !t[j - 1]))) break;

		i++;
		j++;
	}
	return same;
}

//function that induces new names for LMS suffixes. Returns size of new LMS names and if string is unique or not.
pair<bool, int> LmsSubstringNaming(int level, int len, int p1_size, int* sa, char* t, int* p1) {
	int offset = N / level;
	int last = -1;
	int counter = 0;
	for (int i = 0; i < len; i++) {
		if (sa[i] > 0 && t[sa[i]] && !t[sa[i] - 1]) {
			sa[counter++] = sa[i];
		}
	}
	//free second quarter of SA
	for (int i = offset / 2; i < offset; i++)
		sa[i] = -1;
	bool unique = true;
	int name_counter = 0;
	for (int i = 0; i < p1_size; i++) {
		int index = sa[i];
		if (index > 0 && t[index] && !t[index - 1]) {
			if (Compare(level, sa, t, last, index)) {
				//if LMS suffixes are same, give them same name.
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
	//Copy names on the beginning of SA.
	int last_empty = offset / 2;
	for (int i = offset / 2; i < offset; i++) {
		if (sa[i] != -1) {
			sa[last_empty] = sa[i];
			last_empty++;
		}
	}

	return make_pair(unique, name_counter);
}


//SA-IS algorithm function.
int SaIs(int level, int len, int* sa, int* lcp, int num_of_characters, char* t, int *p1) {
	int offset = N / level;
	ClassifyCharacters(level, len, sa, t);
	int p1_size = FindLmsSubstrings(level, len, sa, t, p1);
	InducedSortLmsSubstrings(level, len, num_of_characters, sa, p1, t, lcp);
	pair<bool, int> unique = LmsSubstringNaming(level, len, p1_size, sa, t, p1);
	if (unique.first) {
		//Induce directly.
		for (int i = 0; i < p1_size; i++) {
			sa[sa[i + offset / 2]] = i;
		}

	}
	else {
		FillArray(sa, offset / 2, -1);
		SaIs(2 * level, p1_size, sa, lcp, unique.second, t, p1);	//induce recursively.
	}
	//recompute t and p because they were overwritten.
	ClassifyCharacters(level, len, sa, t);
	p1_size = FindLmsSubstrings(level, len, sa, t, p1);
	InducedSortLmsSubstringsTop(level, len, p1_size, sa, p1, t, lcp, num_of_characters);
	return 0;
}


int main(int argc, const char *argv[]) {
	if (argc != 2) { return -1; }

	std::ifstream inFile;
	inFile.open(argv[1]); //open the input file
	std::stringstream strStream;
	strStream << inFile.rdbuf(); //read the file
	inFile.close();
	string temp = strStream.str();
	int n = temp.length();
	N = temp.length() * 2;

	if (temp[n - 1] == 10) {
		n--;
		N -= 2;
		//linux new line support
	}

	//memory allocation
	int* sa = (int*)malloc(n * sizeof(int) * 2);
	int* lcp = (int*)malloc(n * sizeof(int));
	char* t = (char*)malloc(n * sizeof(char));
	int* p1 = (int*)malloc(n / 2 * sizeof(int));

	for (int i = 0; i < n; i++) {
		sa[i] = -1;
		lcp[i] = -1;
	}
	for (int i = 0; i < n; i++) {
		sa[i + n] = temp[i];
	}

	printf("Starting.\n");
	clock_t start = clock();
	SaIs(2, n, sa, lcp, 256, t, p1);
	clock_t finish = clock();
	cout << "Duration time: " << (double)(finish - start) / (double)CLOCKS_PER_SEC << endl;
	//print_memory_usage();
	FILE* sa_file;
	FILE* lcp_file;
	sa_file = fopen("./sa.txt", "w+");
	lcp_file = fopen("./lcp.txt", "w+");
	for (int i = 0; i < n; i++) {
		fprintf(sa_file, "%d\n", sa[i]);
	}

	for (int i = 0; i < n; i++) {
		fprintf(lcp_file, "%d\n", lcp[i]);
	}

	fclose(sa_file);
	fclose(lcp_file);

	//free memory
	free(sa);
	free(lcp);
	free(t);
	free(p1);

	return 0;
}