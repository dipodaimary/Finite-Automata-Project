#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <stack>
#include <map>

#define DEBUG 1

using namespace std;

/*re to dfa driectly*/
struct tree // Structure for Treenode
    {
    char alpha;
    int pos;
    bool nullable;
    vector<int> fpos, lpos;
    tree *left, *right;
};
int gpos, fl_pos; // for finding  position and follow position index
vector<int> follow_pos[30], state[30]; // follow_pos store the follow position and state store total no of Dstates
map<int, char> alpha_int;
set<char> in_alpha;
vector<int> operator+(vector<int> a, vector<int> b) // operator Overloading for vector Additions
{
    a.insert(a.end(), b.begin(), b.end());
    sort(a.begin(), a.end()); // required for making unique element
    a.erase(unique(a.begin(), a.end()), a.end()); //for finding unique element within vector
    return a;
};
tree* create(char ch, int pos) // Creating Node Memory and initialization
{
    tree* node = new tree;
    node->alpha = ch;
    node->pos = pos;
    node->left = node->right = NULL;
    node->lpos.clear();
    node->fpos.clear();
    return node;
};
class DFA2 {
public:
    bool is_op(char ch)
    {
        if (ch == '|' || ch == '/' || ch == '*' || ch == '.')
            return true;
        return false;
    };

    void vec_print(vector<int> v)
    {
        for (int i = 0; i < v.size(); i++)
            cout << v[i] << " ";
    };

    void postfix(tree* root) // For Traversing The tree
    {
        if (root) {
            string s("          ");
            postfix(root->left);
            postfix(root->right);
            cout << root->alpha << s << root->pos << s << root->nullable << s << "{ ";
            vec_print(root->fpos);
            cout << " }" << s << s << "{ ";
            vec_print(root->lpos);
            cout << " }" << endl;
        }
    };

    void dfa(tree* root, string input) // Finding DFA
    {
        int num_state = 1, cur_state = 1;
        char ch = 'A';
        vector<int> temp;
        map<vector<int>, char> out_state; // Out_state used for removing Redundant States
        map<vector<int>, map<char, vector<int> > > re_dfa; //for Storing The final DFA state
        state[num_state++] = root->fpos; // total number of State , Starting with root's First position
        out_state[root->fpos] = ch++;
        while (1) {
            for (int i = 0; i < input.size(); i++) {
                for (int j = 0; j < state[cur_state].size(); j++) {
                    if (alpha_int[state[cur_state][j]] == input[i])
                        temp = temp + follow_pos[state[cur_state][j]];
                    if (out_state[temp] == 0 && temp.size() > 0) {
                        out_state[temp] = ch++;
                        state[num_state++] = temp;
                    }
                }
                re_dfa[state[cur_state]][input[i]] = temp;
                temp.clear();
            }
            if (cur_state == num_state - 1)
                break;
            cur_state++;
        }
        cout << "The Final State Table :\n\n"; // for Displaying the Final DFA states
        for (auto an : re_dfa) {
            cout << "{ ";
            for (auto jn : an.first)
                cout << jn << " ";
            cout << " } ";
            for (auto jn : an.second) {
                cout << " at : " << jn.first << " { ";
                for (auto kn : jn.second)
                    cout << kn << " ";
                cout << " } ";
            }
            cout << endl;
        }
    };
};
/*###################################################################*/

string join(vector<int> v, string delim)
{
    stringstream ss;
    for (int i = 0; i < v.size(); ++i) {
        if (i != 0)
            ss << ",";
        ss << v[i];
    }
    return ss.str();
}

struct trans {
    int vertex_from;
    int vertex_to;
    char trans_symbol;
};

class NFA {
public:
    vector<int> vertex;
    vector<trans> transitions;
    int final_state;

    NFA()
    {
    }

    int get_vertex_count()
    {
        return vertex.size();
    }

    void set_vertex(int no_vertex)
    {
        for (int i = 0; i < no_vertex; i++) {
            vertex.push_back(i);
        }
    }

    void set_transition(int vertex_from, int vertex_to, char trans_symbol)
    {
        trans new_trans;
        new_trans.vertex_from = vertex_from;
        new_trans.vertex_to = vertex_to;
        new_trans.trans_symbol = trans_symbol;
        transitions.push_back(new_trans);
    }

    void set_final_state(int fs)
    {
        final_state = fs;
    }

    int get_final_state()
    {
        return final_state;
    }

    void display()
    {
        trans new_trans;
        cout << "\n";
        for (int i = 0; i < transitions.size(); i++) {
            new_trans = transitions.at(i);
            cout << "q" << new_trans.vertex_from << " -> q" << new_trans.vertex_to << " : Symbol - " << new_trans.trans_symbol << endl;
        }
        cout << "\nThe final state is q" << get_final_state() << endl;
    }

    /**
         * Get the set of reachable state from each specified vertex.
         */
    vector<char> find_possible_input_symbols(const vector<int>& vertexs)
    {
        vector<char> result;

        for (int i = 0; i < vertexs.size(); i++) {
            int vertex_from = vertexs.at(i);
            for (int j = 0; j < transitions.size(); j++) {
                trans it = transitions.at(j);
                if (it.vertex_from == vertex_from && it.trans_symbol != '^') {
                    result.push_back(it.trans_symbol);
                }
            }
        }

        return result;
    }

    vector<int> eclosure(const vector<int>& X)
    {
        vector<int> result;
        vector<bool> visited(get_vertex_count(), false);

        for (int i = 0; i < X.size(); i++) {
            eclosure(X.at(i), result, visited);
        }

        sort(result.begin(), result.end());
        unique(result.begin(), result.end());
#ifdef DEBUG
        cout << "eclosure{" << join(X, ",") << "}  ->  " << join(result, ",") << endl;
#endif
        return result;
    }

    void eclosure(int x, vector<int>& result, vector<bool>& visited)
    {
        result.push_back(x);

        for (int i = 0; i < transitions.size(); i++) {
            trans it = transitions.at(i);
            if (it.vertex_from == x && it.trans_symbol == '^') {
                int y = it.vertex_to;
                if (!visited.at(y)) {
                    visited.at(y) = true;
                    eclosure(y, result, visited);
                }
            }
        }
    }

    vector<int> move(const vector<int>& T, const char trans_symbol)
    {
        vector<int> result;

        for (int j = 0; j < T.size(); j++) {
            int t = T.at(j);

            for (int i = 0; i < transitions.size(); i++) {
                trans it = transitions.at(i);
                if (it.vertex_from == t && it.trans_symbol == trans_symbol) {
                    result.push_back(it.vertex_to);
                }
            }
        }

        sort(result.begin(), result.end());

#ifdef DEBUG
        int l1 = result.size();
        unique(result.begin(), result.end());
        int l2 = result.size();
        if (l2 < l1) {
            cerr << "move(T, a) return non-unique vector" << endl;
            exit(1);
        }
#endif

        return result;
    }
};

NFA concat(NFA a, NFA b)
{
    NFA result;
    result.set_vertex(a.get_vertex_count() + b.get_vertex_count());
    int i;
    trans new_trans;

    for (i = 0; i < a.transitions.size(); i++) {
        new_trans = a.transitions.at(i);
        result.set_transition(new_trans.vertex_from, new_trans.vertex_to, new_trans.trans_symbol);
    }

    result.set_transition(a.get_final_state(), a.get_vertex_count(), '^');

    for (i = 0; i < b.transitions.size(); i++) {
        new_trans = b.transitions.at(i);
        result.set_transition(new_trans.vertex_from + a.get_vertex_count(), new_trans.vertex_to + a.get_vertex_count(), new_trans.trans_symbol);
    }

    result.set_final_state(a.get_vertex_count() + b.get_vertex_count() - 1);

    return result;
}

NFA kleene(NFA a)
{
    NFA result;
    int i;
    trans new_trans;

    result.set_vertex(a.get_vertex_count() + 2);

    result.set_transition(0, 1, '^');

    for (i = 0; i < a.transitions.size(); i++) {
        new_trans = a.transitions.at(i);
        result.set_transition(new_trans.vertex_from + 1, new_trans.vertex_to + 1, new_trans.trans_symbol);
    }

    result.set_transition(a.get_vertex_count(), a.get_vertex_count() + 1, '^');
    result.set_transition(a.get_vertex_count(), 1, '^');
    result.set_transition(0, a.get_vertex_count() + 1, '^');

    result.set_final_state(a.get_vertex_count() + 1);

    return result;
}

NFA or_selection(vector<NFA> selections, int no_of_selections)
{
    NFA result;
    int vertex_count = 2;
    int i, j;
    NFA med;
    trans new_trans;

    for (i = 0; i < no_of_selections; i++) {
        vertex_count += selections.at(i).get_vertex_count();
    }

    result.set_vertex(vertex_count);

    int adder_track = 1;

    for (i = 0; i < no_of_selections; i++) {
        result.set_transition(0, adder_track, '^');
        med = selections.at(i);
        for (j = 0; j < med.transitions.size(); j++) {
            new_trans = med.transitions.at(j);
            result.set_transition(new_trans.vertex_from + adder_track, new_trans.vertex_to + adder_track, new_trans.trans_symbol);
        }
        adder_track += med.get_vertex_count();

        result.set_transition(adder_track - 1, vertex_count - 1, '^');
    }

    result.set_final_state(vertex_count - 1);

    return result;
}

NFA re_to_nfa(string re)
{
    stack<char> operators;
    stack<NFA> operands;
    char op_sym;
    int op_count;
    char cur_sym;
    NFA* new_sym;

    for (string::iterator it = re.begin(); it != re.end(); ++it) {
        cur_sym = *it;
        if (cur_sym != '(' && cur_sym != ')' && cur_sym != '*' && cur_sym != '|' && cur_sym != '.') {
            new_sym = new NFA();
            new_sym->set_vertex(2);
            new_sym->set_transition(0, 1, cur_sym);
            new_sym->set_final_state(1);
            operands.push(*new_sym);
            delete new_sym;
        }
        else {
            if (cur_sym == '*') {
                NFA star_sym = operands.top();
                operands.pop();
                operands.push(kleene(star_sym));
            }
            else if (cur_sym == '.') {
                operators.push(cur_sym);
            }
            else if (cur_sym == '|') {
                operators.push(cur_sym);
            }
            else if (cur_sym == '(') {
                operators.push(cur_sym);
            }
            else {
                op_count = 0;
                char c;
                op_sym = operators.top();
                if (op_sym == '(')
                    continue;
                do {
                    operators.pop();
                    op_count++;
                } while (operators.top() != '(');
                operators.pop();
                NFA op1;
                NFA op2;
                vector<NFA> selections;
                if (op_sym == '.') {
                    for (int i = 0; i < op_count; i++) {
                        op2 = operands.top();
                        operands.pop();
                        op1 = operands.top();
                        operands.pop();
                        operands.push(concat(op1, op2));
                    }
                }
                else if (op_sym == '|') {
                    selections.assign(op_count + 1, NFA());
                    int tracker = op_count;
                    for (int i = 0; i < op_count + 1; i++) {
                        selections.at(tracker) = operands.top();
                        tracker--;
                        operands.pop();
                    }
                    operands.push(or_selection(selections, op_count + 1));
                }
                else {
                }
            }
        }
    }

    return operands.top();
}

class DFA {
public:
    vector<trans> transitions;
    vector<vector<int> > entries;
    vector<bool> marked;
    vector<int> final_states;

    /**
         * Add newly_created entry into DFA
         */
    int add_entry(vector<int> entry)
    {
        entries.push_back(entry);
        marked.push_back(false);
        return entries.size() - 1;
    }

    /**
         * Return the array position of the next unmarked entry
         */
    int next_unmarked_entry_idx()
    {
        for (int i = 0; i < marked.size(); i++) {
            if (!marked.at(i)) {
                return i;
            }
        }

        return -1;
    }

    /**
         * mark the entry specified by index as marked (marked = true)
         */
    void mark_entry(int idx)
    {
        marked.at(idx) = true;
    }

    vector<int> entry_at(int i)
    {
        return entries.at(i);
    }

    int find_entry(vector<int> entry)
    {
        for (int i = 0; i < entries.size(); i++) {
            vector<int> it = entries.at(i);
            if (it == entry) {
                return i;
            }
        }

        return -1;
    }

    void set_final_state(int nfa_fs)
    {
        for (int i = 0; i < entries.size(); i++) {
            vector<int> entry = entries.at(i);

            for (int j = 0; j < entry.size(); j++) {
                int vertex = entry.at(j);
                if (vertex == nfa_fs) {
                    final_states.push_back(i);
                }
            }
        }
    }

    string get_final_state()
    {
        return join(final_states, ",");
    }

    void set_transition(int vertex_from, int vertex_to, char trans_symbol)
    {
        trans new_trans;
        new_trans.vertex_from = vertex_from;
        new_trans.vertex_to = vertex_to;
        new_trans.trans_symbol = trans_symbol;
        transitions.push_back(new_trans);
    }

    void display()
    {
        trans new_trans;
        cout << "\n";
        for (int i = 0; i < transitions.size(); i++) {
            new_trans = transitions.at(i);
            cout << "q" << new_trans.vertex_from << " {" << join(entries.at(new_trans.vertex_from), ",")
                 << "} -> q" << new_trans.vertex_to << " {" << join(entries.at(new_trans.vertex_to), ",")
                 << "} : Symbol - " << new_trans.trans_symbol << endl;
        }
        cout << "\nThe final state is q : " << join(final_states, ",") << endl;
    }
};

DFA nfa_to_dfa(NFA nfa)
{
    DFA dfa;

    const vector<int> start(1, 0);
    const vector<int> s0 = nfa.eclosure(start);

    int vertex_from = dfa.add_entry(s0);

    while (vertex_from != -1) {
        const vector<int> T = dfa.entry_at(vertex_from);
        dfa.mark_entry(vertex_from);

        const vector<char> symbols = nfa.find_possible_input_symbols(T);
        for (int i = 0; i < symbols.size(); i++) {
            char a = symbols.at(i);

            //TODO: add a eclosure cache : { state => eclosure }
            const vector<int> U = nfa.eclosure(nfa.move(T, a));

            int vertex_to = dfa.find_entry(U);
            if (vertex_to == -1) { // U not already in S'
                vertex_to = dfa.add_entry(U);
            }

            dfa.set_transition(vertex_from, vertex_to, a);
        }

        vertex_from = dfa.next_unmarked_entry_idx();
    }

    // The finish states of the DFA are those which contain any
    // of the finish states of the NFA.
    dfa.set_final_state(nfa.get_final_state());

    return dfa;
}

//Directly converts RE to DFA

int main()
{

    tree* temp;
    stack<tree*> s;
    string str, sp("     "), input;

    string re;
    set<char> symbols;

    cout << "\n\nThe Thompson's Construction Algorithm takes a re as an input "
         << "and returns its corresponding Non-Deterministic/ Denterministic Finite Automaton \n\n";
    cout << "\n\nThe basic building blocks for constructing the NFA are : \n";
    cout << "\n*****\t*****\t*****\n";
    cout << "\nFORMAT : \n"
         << "> Explicitly mention concatenation with a '.' operator \n"
         << "> Enclose every concatenation and or section by parantheses \n"
         << "> Enclose the entire regular expression with parantheses \n\n";

    cout << "For example : \nFor the regular expression (a.(b|c))  -- \n";
    NFA example_nfa = re_to_nfa("(a.(b|c))");
    example_nfa.display();

    cout << "\n\nEnter the regular expression in the above mentioned format: ";
    cin >> re;
    str = re;
    DFA2 dfa2;

    cout<<"********************* RE to NFA *********************"<<endl;

    NFA required_nfa;
    required_nfa = re_to_nfa(re);
    required_nfa.display();
    cout<<"********************* NFA to DFA *********************"<<endl;

    DFA required_dfa = nfa_to_dfa(required_nfa);
    required_dfa.display();

    /*Requirement 3 re to DFA directly*/
    cout<<"********************* RE to DFA directly *********************"<<endl;
    for (int i = 0; i < str.size(); i++) {
        if (!dfa2.is_op(str[i])) {
            gpos++;
            if (str[i] != '#') {
                fl_pos++;
                alpha_int[fl_pos] = str[i];
                in_alpha.insert(str[i]);
            }
            temp = create(str[i], gpos);
            temp->nullable = false;
            temp->fpos.push_back(gpos);
            temp->lpos.push_back(gpos);
        }
        else if (str[i] == '*') {
            temp = create(str[i], 0);
            temp->left = s.top(), s.pop();
            temp->nullable = true;
            temp->fpos = temp->left->fpos;
            temp->lpos = temp->left->lpos;
            for (int i = 0; i < temp->lpos.size(); i++)
                follow_pos[temp->lpos[i]] = follow_pos[temp->lpos[i]] + temp->fpos;
        }
        else if (str[i] == '.') {
            temp = create(str[i], 0);
            temp->right = s.top(), s.pop();
            temp->left = s.top(), s.pop();
            temp->nullable = temp->right->nullable && temp->left->nullable;
            if (temp->left->nullable)
                temp->fpos = temp->right->fpos + temp->left->fpos;
            else
                temp->fpos = temp->left->fpos;
            if (temp->right->nullable)
                temp->lpos = temp->right->lpos + temp->left->lpos;
            else
                temp->lpos = temp->right->lpos;
            for (int i = 0; i < temp->left->lpos.size(); i++)
                follow_pos[temp->left->lpos[i]] = follow_pos[temp->left->lpos[i]] + temp->right->fpos;
        }
        else {
            temp = create(str[i], 0);
            temp->right = s.top(), s.pop();
            temp->left = s.top(), s.pop();
            temp->nullable = temp->right->nullable && temp->left->nullable;
            temp->fpos = temp->right->fpos + temp->left->fpos;
            temp->lpos = temp->right->lpos + temp->left->lpos;
        }
        s.push(temp);
    }
    for (auto temp : in_alpha)
        input.push_back(temp);
    cout << "NODE" << sp << "Position"
         << "       "
         << "Nullable"
         << "        "
         << "First position"
         << "           "
         << "Last position" << endl;
    dfa2.postfix(temp);
    cout << "Follow Position" << endl;
    for (int i = 1; i <= fl_pos; i++) // Display of Follow Position
    {
        cout << i << sp << alpha_int[i] << sp << "{ ";
        for (int j = 0; j < follow_pos[i].size(); j++) {
            cout << follow_pos[i][j] << " ";
        }
        cout << " }\n";
    }
    dfa2.dfa(temp, input);

    return 0;
}