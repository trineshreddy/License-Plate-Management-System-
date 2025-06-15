#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

// Enum for node color in the Red-Black Tree
enum Color { RED, BLACK };

// Node structure for the Red-Black Tree
struct Node {
    string plateNum;     // License plate number (used as key)
    bool is_custom;      // Indicates if it's a custom plate (true) or randomly generated (false)
    Color color;         // RED or BLACK for balancing
    Node *left, *right, *parent; // Pointers to left/right child and parent

    Node(string plate, bool custom)
        : plateNum(plate), is_custom(custom), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

// Class to manage the Red-Black Tree and license plate operations
class RedBlackTree {
private:
    Node* root;      // Root of the tree
    Node* TNULL;     // Sentinel null node (acts as leaves for balancing)
    int revenue;     // Tracks total revenue

    // Initializes a TNULL (black leaf) node
    void initializeNULLNode(Node* node, Node* parent) {
        node->plateNum = "";
        node->is_custom = false;
        node->color = BLACK;
        node->left = nullptr;
        node->right = nullptr;
        node->parent = parent;
    }

    // Searches for a node by license plate
    Node* searchTreeHelper(Node* node, const string& key) {
        if (node == TNULL || key == node->plateNum) return node;
        if (key < node->plateNum) return searchTreeHelper(node->left, key);
        return searchTreeHelper(node->right, key);
    }

    // Inorder traversal to get plates within a range
    void inorderRange(Node* node, const string& lo, const string& hi, vector<string>& result) {
        if (node == TNULL) return;
        if (lo < node->plateNum) inorderRange(node->left, lo, hi, result);
        if (lo <= node->plateNum && node->plateNum <= hi) result.push_back(node->plateNum);
        if (hi > node->plateNum) inorderRange(node->right, lo, hi, result);
    }

    // Performs left rotation (used in balancing)
    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != TNULL) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // Performs right rotation (used in balancing)
    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != TNULL) y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

        // Fixes Red-Black Tree violations after insertion
    void insertFix(Node* k) {
        Node* u;
        while (k->parent && k->parent->color == RED) {
            if (k->parent == k->parent->parent->right) {
                u = k->parent->parent->left;
                if (u && u->color == RED) {
                    // Case 1: uncle is red
                    k->parent->color = BLACK;
                    u->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    // Case 2 & 3: uncle is black
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    leftRotate(k->parent->parent);
                }
            } else {
                // Mirror case for left child
                u = k->parent->parent->right;
                if (u && u->color == RED) {
                    k->parent->color = BLACK;
                    u->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        leftRotate(k);
                    }
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    rightRotate(k->parent->parent);
                }
            }
            if (k == root) break;
        }
        root->color = BLACK;
    }

    // Replaces one subtree with another (used in deletion)
    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    // Returns the node with the smallest key in a subtree
    Node* minimum(Node* node) {
        while (node->left != TNULL) node = node->left;
        return node;
    }

    // Fixes violations after deletion
    void deleteFix(Node* x) {
        Node* s;
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                s = x->parent->right;
                if (s->color == RED) {
                    s->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    s = x->parent->right;
                }
                if (s->left->color == BLACK && s->right->color == BLACK) {
                    s->color = RED;
                    x = x->parent;
                } else {
                    if (s->right->color == BLACK) {
                        s->left->color = BLACK;
                        s->color = RED;
                        rightRotate(s);
                        s = x->parent->right;
                    }
                    s->color = x->parent->color;
                    x->parent->color = BLACK;
                    s->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                // Mirror case
                s = x->parent->left;
                if (s->color == RED) {
                    s->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    s = x->parent->left;
                }
                if (s->right->color == BLACK && s->left->color == BLACK) {
                    s->color = RED;
                    x = x->parent;
                } else {
                    if (s->left->color == BLACK) {
                        s->right->color = BLACK;
                        s->color = RED;
                        leftRotate(s);
                        s = x->parent->left;
                    }
                    s->color = x->parent->color;
                    x->parent->color = BLACK;
                    s->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    // Deletes a node from the tree by plate number
    void deleteNodeHelper(Node* node, const string& key) {
        Node* z = TNULL;
        Node* x, *y;
        while (node != TNULL) {
            if (node->plateNum == key) z = node;
            if (node->plateNum <= key) node = node->right;
            else node = node->left;
        }
        if (z == TNULL) return; // Not found

        y = z;
        Color y_original_color = y->color;
        if (z->left == TNULL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == TNULL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) x->parent = y;
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (y_original_color == BLACK) deleteFix(x);
    }

    public:
    // Constructor initializes the tree and revenue
    RedBlackTree() {
        TNULL = new Node("", false);
        TNULL->color = BLACK;
        TNULL->left = TNULL->right = nullptr;
        root = TNULL;
        revenue = 0;
        srand(time(0)); // Seed for random plate generation
    }

    // Adds a license plate: custom if plateNum is provided, otherwise random
    string addLicence(string plateNum = "") {
        bool is_custom = !plateNum.empty();

        // Randomly generate plate if not custom
        if (!is_custom) {
            do {
                plateNum = "";
                for (int i = 0; i < 4; ++i) {
                    int r = rand() % 36;
                    plateNum += (r < 10) ? ('0' + r) : ('A' + r - 10);
                }
            } while (searchTreeHelper(root, plateNum) != TNULL);
        }

        // Reject duplicate custom plates
        if (searchTreeHelper(root, plateNum) != TNULL) {
            if (is_custom) return "Failed to register " + plateNum + ": already exists.";
        } else {
            // Insert into tree
            Node* node = new Node(plateNum, is_custom);
            node->left = node->right = TNULL;

            Node* y = nullptr;
            Node* x = root;

            while (x != TNULL) {
                y = x;
                if (node->plateNum < x->plateNum) x = x->left;
                else x = x->right;
            }

            node->parent = y;
            if (!y) root = node;
            else if (node->plateNum < y->plateNum) y->left = node;
            else y->right = node;

            if (!node->parent) node->color = BLACK;
            else insertFix(node);

            revenue += is_custom ? 7 : 4;
            return is_custom ? plateNum + " registered successfully." : plateNum + " created and registered successfully.";
        }
        return "";
    }

    // Removes a license plate if it exists
    string dropLicence(const string& plateNum) {
        Node* found = searchTreeHelper(root, plateNum);
        if (found == TNULL) return "Failed to remove " + plateNum + ": does not exist.";
        deleteNodeHelper(root, plateNum);
        revenue -= found->is_custom ? 7 : 4;
        return plateNum + " removed successfully.";
    }

    // Checks if a license plate exists
    string lookupLicence(const string& plateNum) {
        return searchTreeHelper(root, plateNum) != TNULL ? plateNum + " exists." : plateNum + " does not exist.";
    }

    // Finds the lexicographically previous plate
    string lookupPrev(const string& plateNum) {
        string result = "";
        Node* current = root;
        while (current != TNULL) {
            if (current->plateNum < plateNum) {
                result = current->plateNum;
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return plateNum + "'s prev is " + (result.empty() ? "." : result + ".");
    }

    // Finds the lexicographically next plate
    string lookupNext(const string& plateNum) {
        string result = "";
        Node* current = root;
        while (current != TNULL) {
            if (current->plateNum > plateNum) {
                result = current->plateNum;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return plateNum + "'s next is " + (result.empty() ? "." : result + ".");
    }

    // Lists all plates between lo and hi
    string lookupRange(const string& lo, const string& hi) {
        vector<string> result;
        inorderRange(root, lo, hi, result);
        string out = "plate numbers between " + lo + " and " + hi + ": ";
        for (size_t i = 0; i < result.size(); ++i) {
            out += result[i];
            if (i != result.size() - 1) out += ", ";
        }
        return out + ".";
    }

    // Returns total revenue generated
    string revenueReport() {
        return "Current annual revenue is " + to_string(revenue) + " Galleons.";
    }
};

// Parses and executes commands from the input file
void processCommands(const string& filename) {
    RedBlackTree tree;
    ifstream infile(filename);
    ofstream outfile(filename + "_output.txt");
    string line;

    while (getline(infile, line)) {
        if (line.empty()) continue;

        string cmd, arg1 = "", arg2 = "";
        size_t openParen = line.find('(');
        size_t closeParen = line.find(')');
    
        if (openParen == string::npos || closeParen == string::npos || closeParen <= openParen)
            return; // Invalid format
    
        cmd = line.substr(0, openParen);
        string paramStr = line.substr(openParen + 1, closeParen - openParen - 1);

        size_t comma = paramStr.find(',');
        if (comma != string::npos) {
            arg1 = paramStr.substr(0, comma);
            arg2 = paramStr.substr(comma + 1);
        } else {
            arg1 = paramStr;
        }

        // Trim helper
        auto trim = [](string& s) {
            size_t start = s.find_first_not_of(" \t");
            size_t end = s.find_last_not_of(" \t");
            s = (start == string::npos) ? "" : s.substr(start, end - start + 1);
        };
        trim(cmd); trim(arg1); trim(arg2);

        // Execute the appropriate command
        if (cmd == "addLicence") {
            if (!arg1.empty()) outfile << tree.addLicence(arg1) << endl;
            else outfile << tree.addLicence() << endl;
        } else if (cmd == "dropLicence") {
            outfile << tree.dropLicence(arg1) << endl;
        } else if (cmd == "lookupLicence") {
            outfile << tree.lookupLicence(arg1) << endl;
        } else if (cmd == "lookupPrev") {
            outfile << tree.lookupPrev(arg1) << endl;
        } else if (cmd == "lookupNext") {
            outfile << tree.lookupNext(arg1) << endl;
        } else if (cmd == "lookupRange") {
            outfile << tree.lookupRange(arg1, arg2) << endl;
        } else if (cmd == "revenue") {
            outfile << tree.revenueReport() << endl;
        } else if (cmd == "quit") {
            break;
        }
    }
}

// Entry point: reads input filename from command line and processes it
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./plateMgmt inputFileName" << endl;
        return 1;
    }
    processCommands(argv[1]);
    return 0;
}
