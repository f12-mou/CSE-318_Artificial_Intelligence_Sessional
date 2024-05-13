#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define N 1728

vector<int> vec[8];			  // for tabulating 7*1728 data elements. 6 columns+1result
vector<int> attr_vec[8];	  // possible attribute values against all the input attributes
map<string, int> globalNames; // everything is hashmapped into integer

// for experimentation
int total_correct_ans;
int total_wrong_ans;
bool isConsistent; // marked true if at least one row of data is inconsistent
int startNo;	   // starting node number of DT

// calculating metrics
vector<int> correct_answers;
vector<int> wrong_answers;

int now_correct_ans;
int now_wrong_ans;

// Decision Tree Nodes
class node
{
public:
	int attribute_id;			// attribute to test: -1 if the node is a leaf node
	int parent_node;			// parent node number
	map<int, int> nextBranches; // <attribute_value, next_node_no>
	int plurality;				// my plurality:calculated once to reuse later on demand
	int is_leaf;				// whether the node is a leaf
	int total_data;				// for testing the quality of DT: whether a leaf contains fewer examples

	// setting the attributes
	void initialize(int a_id, int p_node, int p, int is_l)
	{
		attribute_id = a_id;
		parent_node = p_node;
		plurality = p;
		is_leaf = is_l;
	}
	// reset before each iteration
	void reset()
	{
		attribute_id = 0;
		parent_node = 0;
		plurality = 0;
		is_leaf = 0;
		nextBranches.clear();
	}
	int getPlurality()
	{
		return plurality;
	}
} nodes[100005];

// returns the mostly occuring decision value in the set
int getPlurality(set<int> indices)
{
	map<int, int> mp; // index_number, total_count_of_that_index_result
	for (auto itr : indices)
	{
		int nowValue = vec[7][itr]; // vector 7 contains the decision values
		mp[nowValue]++;
	}
	int finalResult;
	int count = 0;
	for (auto itr : mp)
	{
		int now_attr_val = itr.first;
		int now_count = itr.second;
		if (now_count >= count)
		{
			count = now_count;
			finalResult = now_attr_val;
		}
	}
	return finalResult;
}

// whether the example set is pure:contains only one decision value
bool check_for_leaf_node(set<int> indices)
{
	set<int> st;
	for (auto itr : indices)
	{
		st.insert(vec[7][itr]); // vector 7 contains the decision values
	}
	if (st.size() == 1)
		return true;
	else
		return false;
}

// DFS : returns the decision value based on input vector attributes(inputArr)
int getDecision(set<pair<int, int>> inputArr, int nodeNo)
{
	if (nodes[nodeNo].is_leaf == 1)
	{
		return nodes[nodeNo].plurality; // return the decision value
	}
	int test_attr_id_now = nodes[nodeNo].attribute_id; // which attr to test now
	int search_attr_no = -1;
	for (auto itr : inputArr)
	{
		int firstVal = itr.first;
		int secVal = itr.second;
		if (firstVal == test_attr_id_now)
		{
			search_attr_no = secVal;
			break;
		}
	}
	for (auto itr : nodes[nodeNo].nextBranches)
	{
		int key_val = itr.first;
		if (nodes[nodeNo].nextBranches[search_attr_no] == 0) // not found any example before
		{
			return nodes[nodeNo].plurality; // I do not know where to go, I return my plurality
		}
		else if (nodes[nodeNo].nextBranches[search_attr_no] == -1)
		{
			return nodes[nodeNo].plurality;
		}
		else
		{
			return getDecision(inputArr, nodes[nodeNo].nextBranches[search_attr_no]);
		}
	}
}

double getEntropy(set<int> indices, int attr_id)
{
	double total_val = 0.0;
	for (int i = 1; i < vec[attr_id].size(); i++)
	{
		int target_val_now = vec[attr_id][i];
		set<int> target_indices_now;
		for (auto itr : indices)
		{
			if (vec[attr_id][itr] == target_val_now)
			{
				target_indices_now.insert(itr);
			}
		}
		double total_now_indices = 0.0;
		map<int, int> count_now;
		for (auto itr : target_indices_now)
		{
			// int now_who = vec[attr_id][itr];
			int now_who = vec[7][itr];
			count_now[now_who]++;
		}
		// done with sampling in one partitioned node
		double temp_total_now = 0.0;
		for (auto itr : count_now)
		{
			double here_count = (double)itr.second;
			temp_total_now = temp_total_now + ((here_count) / target_indices_now.size()) * log2((target_indices_now.size() / here_count));
		}
		total_val = total_val + temp_total_now * ((double)target_indices_now.size() / (double)indices.size());
		// cout << total_val << endl;
	}
	return total_val;
}

double parentEntropy(set<int> indices)
{
	double total_val = 0.0;
	map<int, int> count_now;
	for (auto itr : indices)
	{
		int now_who = vec[7][itr];
		count_now[now_who]++;
	}
	// done with sampling in one partitioned node
	double temp_total_now = 0.0;
	for (auto itr : count_now)
	{
		double here_count = (double)itr.second;
		temp_total_now = temp_total_now + ((here_count) / indices.size()) * log2((indices.size() / here_count));
	}
	total_val = total_val + temp_total_now * ((double)1.0); // same set
	return total_val;
}

// Definition of input attributes
// indices: initial set of indices received
// rem_attr: attribute set remaining to choose for
// parent_node: making chain
// returns the nodeNo back to parent, so that parent can map it against selected
// attribute value
int createDecisionTree(set<int> &indices, set<int> &rem_attr, int parent_node)
{
	if (indices.size() == 0 && rem_attr.size() == 0)
	{
		return -1;
	}
	else if (indices.size() == 0) // example set is empty
	{
		// new node returning the plurality of parent
		nodes[startNo].initialize(-1, parent_node, nodes[parent_node].getPlurality(), 1);
		startNo++;
		return startNo - 1;
	}
	else if (rem_attr.size() == 0) // No remaining attributes, should be a leaf node with one type of results
	{
		int my_plurality = getPlurality(indices);
		nodes[startNo].initialize(-1, parent_node, my_plurality, 1);
		// leaf e koita examples
		nodes[startNo].total_data = indices.size();
		startNo++;
		// checking for consistency
		if (check_for_leaf_node(indices) == false)
		{
			isConsistent = false; // the leaf was supposed to hold only one decision val
		}
		return startNo - 1;
	}
	else if (check_for_leaf_node(indices))
	{
		// becomes a leaf node
		int my_plurality = getPlurality(indices);
		nodes[startNo].initialize(-1, parent_node, my_plurality, 1);
		nodes[startNo].total_data = indices.size();
		startNo++;
		return startNo - 1;
	}
	else
	{
		// have to sort by entropy
		double maxi_gain = -1e9;
		int final_attr_id;
		double parent_entropy = parentEntropy(indices);
		for (auto itr : rem_attr)
		{
			double now_entropy = getEntropy(indices, itr); // let's check indices and corresponding attribute
			double info_gain = parent_entropy - now_entropy;
			if (info_gain >= maxi_gain)
			{
				maxi_gain = info_gain;
				final_attr_id = itr;
			}
		}
		// done selecting proper attribute
		int p = getPlurality(indices);
		nodes[startNo].initialize(final_attr_id, parent_node, p, 0);
		int return_val = startNo;
		startNo++;
		// expand children
		for (int i = 1; i < attr_vec[final_attr_id].size(); i++)
		{
			int target_val_now = attr_vec[final_attr_id][i];
			set<int> target_indices_now;
			for (auto itr : indices)
			{
				if (vec[final_attr_id][itr] == target_val_now)
				{
					target_indices_now.insert(itr);
				}
			}
			// akta possible value er against e values pacchchi
			set<int> now_rem_attr = rem_attr;
			now_rem_attr.erase(final_attr_id);
			int child_return_val = createDecisionTree(target_indices_now, now_rem_attr, return_val);
			nodes[return_val].nextBranches[target_val_now] = child_return_val;
		}
		return return_val;
	}
}

void readData()
{
	for (int i = 0; i <= 7; i++)
	{
		vec[i].push_back(-1);
		attr_vec[i].push_back(-1);
	}
	FILE *fp = freopen("car.data", "r", stdin);
	string data, intermediate;
	int tokenNo = 1;
	map<int, int> mp[8];
	for (int i = 1; i <= N; i++)
	{
		cin >> data;
		vector<string> tokens;
		stringstream check1(data);
		while (getline(check1, intermediate, ','))
		{
			tokens.push_back(intermediate);
		}
		for (int j = 0; j < tokens.size(); j++)
		{
			string now = tokens[j];
			if (globalNames[now] == 0)
			{
				globalNames[now] = tokenNo;
				if (j <= 5)
				{
					mp[j + 1][tokenNo]++;
				}
				tokenNo++;
			}
			else
			{
				int nowHehe = globalNames[now];
				if (j <= 5)
				{
					mp[j + 1][nowHehe]++;
				}
			}
			vec[j + 1].push_back(globalNames[now]);
		}
	}

	fclose(fp);
	for (int i = 1; i <= 6; i++)
	{
		for (auto itr : mp[i])
		{
			int now = itr.first;
			attr_vec[i].push_back(now);
		}
	}
}

void testData(set<int> indices)
{
	now_correct_ans = 0;
	now_wrong_ans = 0;
	for (auto itr : indices)
	{
		set<pair<int, int>> input;
		for (int i = 1; i <= 6; i++)
		{
			input.insert({i, vec[i][itr]});
		}
		int result = getDecision(input, 1);
		int ideal_result = vec[7][itr];
		if (result == ideal_result)
		{
			now_correct_ans++;
		}
		else
		{
			now_wrong_ans++;
		}
	}
	total_correct_ans += now_correct_ans;
	total_wrong_ans += now_wrong_ans;
	// for evaluating standard deviation
	correct_answers.push_back(now_correct_ans);
	wrong_answers.push_back(now_wrong_ans);
}

void prepareData()
{
	set<int> indices, rem_attr;
	int no_test_data = (int)((0.2) * (double)(N * 1.0));
	for (int i = 1; i <= N; i++)
	{
		indices.insert(i);
	}
	for (int i = 1; i <= 6; i++)
	{
		rem_attr.insert(i);
	}

	for (int i = 1; i <= 20; i++)
	{
		set<int> training_data = indices;
		set<int> training_rem_attr = rem_attr;
		set<int> test_data;
		int max_rand = indices.size();
		for (int j = 1; j <= no_test_data; j++)
		{
			int index_for_testing = rand() % max_rand;
			auto it = next(training_data.begin(), index_for_testing);
			test_data.insert(*it);
			auto it2 = next(training_data.begin(), index_for_testing);
			training_data.erase(it2);
			max_rand--;
		}
		for (int i = 0; i < 100005; i++)
		{
			nodes[i].reset();
		}
		startNo = 1;
		createDecisionTree(training_data, training_rem_attr, 0);
		// tree built
		testData(test_data);
		cout << "Completed iteration no - " << i << endl;
	}
}

void calculateMetrics()
{
	cout << "Number of nodes in the decision tree " << startNo - 1 << endl;
	cout << "Number of iterations run on the 20% examples(test set): 20" << endl;
	cout << endl;
	cout << "The list showing results--------------------------" << endl;
	cout << endl;
	vector<double> accuracy;
	cout << "Iteration no  "
		 << "    no of correct answers "
		 << "		no of wrong answers"
		 << "		Percentage of accuracy" << endl;
	for (int i = 1; i <= 20; i++)
	{
		double now = (100 * correct_answers[i - 1]) / (correct_answers[i - 1] + wrong_answers[i - 1]);
		accuracy.push_back(now);
		cout
			<< i << "		" << correct_answers[i - 1] << "		" << wrong_answers[i - 1] << "	" << accuracy[i - 1] << endl;
	}

	// Mean accuracy
	double total_accuracy = 0.0;
	double mean_accuracy = 0.0;
	for (int i = 1; i <= 20; i++)
	{
		total_accuracy += accuracy[i - 1];
	}
	mean_accuracy = (total_accuracy / 20);
	// standard deviation
	double square_deviation = 0.0;
	for (int i = 1; i <= 20; i++)
	{
		square_deviation += ((mean_accuracy - accuracy[i - 1]) * (mean_accuracy - accuracy[i - 1]));
	}
	double standard_deviation = sqrt((square_deviation / 20));
	cout << endl;
	cout << "---------------------------------------" << endl;
	cout << "Performance Metrics:" << endl;
	cout << "Mean Accuracy (%): " << mean_accuracy << endl;
	cout << "Standard Deviation: " << standard_deviation << endl;
	cout << endl;
	cout << endl;
}

void prepareGraph()
{
	set<int> indices, rem_attr;
	for (int i = 1; i <= N; i++)
	{
		indices.insert(i);
	}
	for (int i = 1; i <= 6; i++)
	{
		rem_attr.insert(i);
	}
	int start = 95;
	FILE *fp = freopen("graph.csv", "w", stdout);
	cout << "training_data(%),accuracy(%)" << endl;
	cout << "0,0" << endl;
	for (int i = 1; i <= 19; i++)
	{
		set<int> training_data = indices;
		set<int> training_rem_attr = rem_attr;
		set<int> test_data;
		int max_rand = indices.size();
		int no_test_data = (int)(((double)start / (double)100.0) * (double)(N * 1.0));
		for (int j = 1; j <= no_test_data; j++)
		{
			int index_for_testing = rand() % max_rand;
			auto it = next(training_data.begin(), index_for_testing);
			test_data.insert(*it);
			auto it2 = next(training_data.begin(), index_for_testing);
			training_data.erase(it2);
			max_rand--;
			if (max_rand == 0)
				break;
		}
		for (int i = 0; i < 100005; i++)
		{
			nodes[i].reset();
		}
		startNo = 1;
		createDecisionTree(training_data, training_rem_attr, 0);
		// tree built
		testData(test_data);
		double eff = ((double)now_correct_ans) / (double)((double)now_correct_ans + (double)now_wrong_ans);
		cout
			<< (100 - start) << "," << eff << endl;
		start -= 5;
	}
	cout << "100,1.0" << endl;
	fclose(fp);
}

int main()
{
	readData();
	prepareData();
	calculateMetrics();
	cout << "Preparing data for Performance Graph varying training data (5% - 100%)" << endl;
	prepareGraph();
	return 0;
}
