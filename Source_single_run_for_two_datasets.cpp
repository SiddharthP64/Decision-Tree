//ML Assignment 1 Feb 8 2015
//Siddharth Perumal
//sxp132830

#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<queue>
#include<time.h>
#include<cstdlib>
#include<cmath>

using namespace std;

//Output file to store the output.
ofstream myfile;

//Structure to form the tree.
struct node
{
	int id;
	int classValue;
	node *leftChild;
	node *rightChild;
};
node *head = NULL;

void readFile(vector <vector <double> > & source, string fileName)
{
	double convertor;
	int numLines = 0, numColm = 1;
	string totalPath = "DS" + fileName;
	ifstream file(totalPath.c_str());
	stringstream iss;
	string line, word;

	while (getline(file, line))
	{
		iss << line;

		source.push_back(vector <double>());
		numColm = 1;
		while (getline(iss, word, ','))
		{
			stringstream tempStream(word);
			tempStream >> convertor;
			if (numLines != 0)
			{
				source[numLines].push_back(convertor);
			}
			else
			{
				source[numLines].push_back(numColm);
			}
			numColm++;
		}
		numLines++;
		iss.clear();

	}
	file.close();
}

void decisionTreeBuilder(vector<vector<double> > input, int level, node * &current);

int targetDecision(vector<double> input, node * current)
{
	double colName;
	colName = current->id;
	int leftChildDecision = -1, rightChildDecision = -1;
	if (colName != 0)
	{
		if (input[colName-1] == 1)
		{
			leftChildDecision = targetDecision(input, current->leftChild);
		}
		else if (input[colName-1] == 0)
		{
			rightChildDecision = targetDecision(input, current->rightChild);
		}
	}
	else if (colName == 0)
	{
		return current->classValue;
	}
	if (leftChildDecision != -1)
		return leftChildDecision;
	else
		return rightChildDecision;
}

double accuracyCalulator(vector<vector<double> > input, node * current)
{
	int rowCount = input.size();
	int colmnCount = input[0].size();
	double correctDecision = 0;
	double accuracy = 0;
	int decisionVal;
	for (int i = 1; i < rowCount; i++)
	{
		decisionVal = targetDecision(input[i], current);
		if (decisionVal == input[i][colmnCount - 1])
		{
			correctDecision++;
		}
	}

	accuracy = (correctDecision / (rowCount - 1) * 100);
	return accuracy;
}

void printTree(node * current, string dash)
{
	if (current != NULL)
	{
		if (current->id > 0)
		{
			cout << endl << dash << "X" << char(65 + current->id) << " = " << "1" << " : ";
			myfile << endl << dash << "X" << char(65 + current->id) << " = " << "1" << " : ";
			dash = dash + "| ";
			printTree(current->leftChild, dash);
			dash = dash.substr(0, dash.size() - 2);
			cout << endl << dash << "X" << char(65 + current->id) << " = " << "0" << " : ";
			myfile << endl << dash << "X" << char(65 + current->id) << " = " << "0" << " : ";
			dash = dash + "| ";
			printTree(current->rightChild, dash);
		}
		else
		{
			cout << current->classValue;
			myfile << current->classValue;
		}
	}
	else
		return;
}

void copyTree(node * &copiedTree, node * sourceTree)
{
	if (sourceTree == NULL)
		copiedTree = NULL;
	else
	{
		copiedTree = new node;
		copiedTree->classValue = sourceTree->classValue;
		copiedTree->id = sourceTree->id;
		copyTree(copiedTree->leftChild, sourceTree->leftChild);
		copyTree(copiedTree->rightChild, sourceTree->rightChild);
	}
}

int nodeCount(node * source)
{
	int count = 0;
	node * accessVar;
	queue<node*> BFS;
	BFS.push(source);
	while (!BFS.empty())
	{
		accessVar = BFS.front();
		BFS.pop();
		count++;
		
		if (accessVar->leftChild->id != 0)
			BFS.push(accessVar->leftChild);
		if (accessVar->rightChild->id != 0)
			BFS.push(accessVar->rightChild);
	}
	return count;
}

void nodeDeletion(node * &source, int position)
{
	if (source != NULL)
	{
		int count = 0;
		node * accessVar = source;
		queue<node*> BFS;
		BFS.push(source);
		while (!BFS.empty())
		{
			accessVar = BFS.front();
			BFS.pop();
			if (++count == position)
				break;
			if (accessVar->leftChild->id != 0)
				BFS.push(accessVar->leftChild);
			if (accessVar->rightChild->id != 0)
				BFS.push(accessVar->rightChild);
		}
		accessVar->id = 0;
	}
}

node * postPruning(int L, int K, node * sourceTree, string fileName)
{
	node *Dprime, *Dbest;
	vector<vector<double> > source;
	double accuracyDprime, accuracyDbest;

	readFile(source, fileName);
	Dbest = sourceTree;
	accuracyDbest = accuracyCalulator(source, Dbest);
	for (int i = 1; i <= L; i++)
	{
		copyTree(Dprime, sourceTree);
		int M = (rand() % K )+ 1;
		for (int j = 1; j < M; j++)
		{
			//Counting the number of non leaf nodes.
			int N = nodeCount(Dprime);
			int P = (rand() % N) + 1;
			nodeDeletion(Dprime, P);
		}
		accuracyDprime = accuracyCalulator(source, Dprime);
		if (accuracyDprime > accuracyDbest)
		{
			Dbest = NULL;
			copyTree(Dbest, Dprime);
			accuracyDbest = accuracyDprime;
		}
	}
	return Dbest;
}


int main()
{
	int endchar;
	char toStringConverter;
	double accuracy;
	int L, K;
	vector< vector<double> > source;
	srand(time(NULL));
	myfile.open("Output.txt");
	
	cout << "Please enter the pruning variables:" << endl;
	cout << "L :";
	cin >> L;
	cout << "K :";
	cin >> K;
	cout << endl;

	//First dataset.
	readFile(source, "1//training_set.csv");
	decisionTreeBuilder(source, 0, head);

	source.clear();
	readFile(source, "1//test_set.csv");
	accuracy = accuracyCalulator(source, head);
	cout << "Accuracy for test dataset1 before pruning= " << accuracy << "%" << endl;
	myfile << "Accuracy for test dataset1 before pruning = " << accuracy << "%" << endl << endl;

	head = postPruning(L, K, head, "1//validation_set.csv");
	accuracy = accuracyCalulator(source, head);
	cout << "Accuracy for test dataset1 after pruning = " << accuracy << "%" << endl;
	myfile << "Accuracy for test dataset1 after pruning = " << accuracy << "%" << endl << endl;
	
	myfile << "Tree structure for the first data set post pruning: " << endl;
	printTree(head, "");

	//Second dataset
	source.clear();
	readFile(source, "2//training_set.csv");
	decisionTreeBuilder(source, 0, head);
	
	source.clear();
	readFile(source, "2//test_set.csv");
	accuracy = accuracyCalulator(source, head);
	cout << "\n\nAccuracy for test dataset2  before pruning = " << accuracy << "%" << endl;
	myfile << "\n\nAccuracy for test dataset2 before pruning = " << accuracy << "%" << endl << endl;

	head = postPruning(L, K, head, "2//validation_set.csv");
	accuracy = accuracyCalulator(source, head);
	cout << "Accuracy for test dataset2 after pruning = " << accuracy << "%" << endl;
	myfile << "Accuracy for test dataset2 after pruning = " << accuracy << "%" << endl << endl;
	
	myfile << "Tree structure for the second data set post pruning: " << endl;
	printTree(head, "");

	source.clear();

	myfile.close();
	cout << "Press any key to exit."<<endl;
	cin >> endchar;
	return 0;
}

void decisionTreeBuilder(vector<vector<double> > input, int level, node * &current)
{
	if (input.size() > 2)//Check for the case of just one Column??
	{
		vector<vector<double> > resultCount(20, vector<double>(4));
		double informationGain[20];
		double gainRatio[20];
		int classDefL, classDefR;

		int callLeft = 1, callRight = 1;
		double S = 0, pos = 0, neg = 0;
		double modS = input.size() - 1;
		double modSvT, modSvF, SVT, SVF;
		double truePosFraction = 0, trueNegFraction = 0, falsePosFraction = 0, falseNegFraction = 0;

		int colmnCount = input[0].size() - 1, rowCount = modS + 1;
		for (int j = 0; j < colmnCount; j++)
		{
			for (int i = 1; i < rowCount; i++)
			{
				if (input[i][j] == 1)
				{
					if (input[i][colmnCount] == 1)
						resultCount[j][0]++;
					else
						resultCount[j][1]++;
				}
				else if (input[i][j] == 0)
				{
					if (input[i][colmnCount] == 1)
						resultCount[j][2]++;
					else
						resultCount[j][3]++;
				}
				else
					cout << "\n***********here\n" << input[i][0] << "  val before and i =" << i;
			}
		}
		//Zero handlers.
		double temp1 = 0, temp2 = 0;
		pos = ((resultCount[0][0] + resultCount[0][2]) / (resultCount[0][0] + resultCount[0][2] + resultCount[0][1] + resultCount[0][3]));
		neg = ((resultCount[0][1] + resultCount[0][3]) / (resultCount[0][0] + resultCount[0][2] + resultCount[0][1] + resultCount[0][3]));
		if (pos == 0)
			temp1 = 0;
		else
			temp1 = (pos*log2(pos));
		if (neg == 0)
			temp2 = 0;
		else
			temp2 = (neg*log2(neg));
		S = -( temp1 + temp2);

		for (int i = 0; i < colmnCount; i++)
		{
			modSvT = resultCount[i][0] + resultCount[i][1];
			modSvF = resultCount[i][2] + resultCount[i][3];

			if (modSvT != 0)
			{
				truePosFraction = (resultCount[i][0] / modSvT);
				trueNegFraction = (resultCount[i][1] / modSvT);
			}
			else
			{
				truePosFraction = 0;
				trueNegFraction = 0;
			}
			if (modSvF != 0)
			{
				falsePosFraction = (resultCount[i][2] / modSvF);
				falseNegFraction = (resultCount[i][3] / modSvF);
			}
			else
			{
				falsePosFraction = 0;
				falseNegFraction = 0;
			}

			double temp1 = 0, temp2 = 0;
			if (truePosFraction == 0)
				temp1 = 0;
			else
				temp1 = (truePosFraction*log2(truePosFraction));
			if (trueNegFraction == 0)
				temp2 = 0;
			else
				temp2 = (trueNegFraction*log2(trueNegFraction));
			SVT = -((modSvT / modS)*(temp1 + temp2));
			if (falsePosFraction == 0)
				temp1 = 0;
			else
				temp1 = (falsePosFraction*log2(falsePosFraction));
			if (falseNegFraction == 0)
				temp2 = 0;
			else
				temp2 = (falseNegFraction*log2(falseNegFraction));
			SVF = -((modSvF / modS)*(temp1 + temp2));

			informationGain[i] = S - (SVT + SVF);
			double S1, S0, Ssum;
			if (modSvT != 0)
				S1 = (modSvT / modS)*log2((modSvT / modS));
			else
				S1 = 0;

			if (modSvF != 0)
				S0 = (modSvF / modS)*log2((modSvF / modS));
			else
				S0 = 0;			
			
			Ssum = -(S1 + S0);
			gainRatio[i] = informationGain[i] / Ssum;
		}

		//to find the attribute with the highest information gain.
		double comparator = 0, attributeindex = 0;
		for (int i = 0; i < colmnCount; i++)
		{
			if (gainRatio[i] > comparator)
			{
				attributeindex = i;
				comparator = gainRatio[i];
			}
		}
		
		//Storing the found node into the tree.
		current = new node;
		current->id = input[0][attributeindex];
		//Storing majority of the decision to be used in pruning.
		if ((resultCount[attributeindex][0] + resultCount[attributeindex][1]) > (resultCount[attributeindex][2] + resultCount[attributeindex][3]))
			current->classValue = 1;
		else
			current->classValue = 0;
		current->leftChild = current->rightChild = NULL;

		if (resultCount[attributeindex][0] == 0 || resultCount[attributeindex][1] == 0)
		{
			callLeft = 0;
			current->leftChild = new node;
			current->leftChild->id = 0;
			current->leftChild->leftChild = current->leftChild->rightChild = NULL;
		}
		
		if (resultCount[attributeindex][2] == 0 || resultCount[attributeindex][3] == 0)
		{
			callRight = 0;
			current->rightChild = new node;
			current->rightChild->id = 0;
			current->rightChild->leftChild = current->rightChild->rightChild = NULL;
		}

		if (resultCount[attributeindex][0] == 0)
			current->leftChild->classValue = 0;
		else if (resultCount[attributeindex][1] == 0)
			current->leftChild->classValue = 1;
		
		if (resultCount[attributeindex][2] == 0)
			current->rightChild->classValue = 0;
		else if (resultCount[attributeindex][3] == 0)
			current->rightChild->classValue = 1;

		//Splitting the source vector.
		vector< vector<double> > trueBranch;
		vector< vector<double> > falseBranch;
		trueBranch.push_back(input[0]);
		falseBranch.push_back(input[0]);
		trueBranch[0].erase(trueBranch[0].begin() + attributeindex);
		falseBranch[0].erase(falseBranch[0].begin() + attributeindex);

		for (int i = 1; i < input.size(); i++)
		{
			if (input[i][attributeindex] == 1)
			{
				trueBranch.push_back(input[i]);
				trueBranch.back().erase(trueBranch.back().begin() + attributeindex);
			}
			else if (input[i][attributeindex] == 0)
			{
				falseBranch.push_back(input[i]);
				falseBranch.back().erase(falseBranch.back().begin() + attributeindex);
			}

		}

		if (callLeft == 1)
			decisionTreeBuilder(trueBranch, ++level, current->leftChild);
		if (callRight == 1)
			decisionTreeBuilder(falseBranch, ++level, current->rightChild);

	}
	else
	{
		cout << "\n\nIn the else part of the recursion." << endl << "\nInput size = " << input.size() << endl;
		myfile << "\n\nIn the else part of the recursion." << endl << "\nInput size = " << input.size() << endl;
		int b;
		cin >> b;
	}
}