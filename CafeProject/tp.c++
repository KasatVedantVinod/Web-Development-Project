#include <iostream>
#include <queue>
using namespace std;

// B-tree node definition
struct bdnode {
    int count = 0;    // Number of keys currently in the node
    int *key;         // Dynamic array of keys
    bdnode **ptr;     // Dynamic array of child pointers
    bdnode *parent = NULL;  // Pointer to the parent node

    // Constructor allocates memory for keys and pointers and initializes pointers to NULL
    bdnode(int d) {
        key = new int[d];
        ptr = new bdnode *[d + 1];
        for (int i = 0; i <= d; i++) 
            ptr[i] = NULL;
    }
};

// Inserts a number into the array of keys while maintaining sorted order
void insertSorted(int arr[], int &n, int num) {
    int i = n - 1;
    while (i >= 0 && arr[i] > num) {
        arr[i + 1] = arr[i];
        i--;
    }
    arr[i + 1] = num;
    n++;
}

// Shifts keys and pointers in a node to insert a new key and corresponding right child pointer.
// This is used when propagating splits upward.
void shiftAndInsert(bdnode *T, int num, bdnode *rightChild) {
    int i = T->count - 1;
    while (i >= 0 && T->key[i] > num) {
        T->key[i + 1] = T->key[i];
        T->ptr[i + 2] = T->ptr[i + 1];
        i--;
    }
    T->key[i + 1] = num;
    T->ptr[i + 2] = rightChild;
    if (rightChild)
        rightChild->parent = T;
    T->count++;
}

// Splits a full node (or child) into two nodes and promotes the middle key upward.
// If the node being split is the root, a new root is created.
void splitNode(bdnode *&T, bdnode *child, int d) {
    int mid = d / 2;      // Find the middle index
    bdnode *right = new bdnode(d);  // Create a new right node

    // Move keys and pointers from child to right node (keys after the middle key)
    for (int i = mid + 1; i < d; i++) {
        right->key[right->count++] = child->key[i];
        right->ptr[right->count - 1] = child->ptr[i];
        if (right->ptr[right->count - 1])
            right->ptr[right->count - 1]->parent = right;
    }
    // Move the last pointer
    right->ptr[right->count] = child->ptr[d];
    if (right->ptr[right->count])
        right->ptr[right->count]->parent = right;

    int middleKey = child->key[mid];
    child->count = mid;   // Adjust the count of keys in the left (original) child

    // If the child being split is the root (has no parent), create a new root.
    if (child->parent == NULL) {
        T = new bdnode(d);
        T->key[0] = middleKey;
        T->count = 1;
        T->ptr[0] = child;
        T->ptr[1] = right;
        child->parent = T;
        right->parent = T;
    } else {
        // Otherwise, insert the middle key and right child into the parent node.
        bdnode *parent = child->parent;
        shiftAndInsert(parent, middleKey, right);
        // If parent's keys are full after insertion, split the parent.
        if (parent->count == d) {
            splitNode(T, parent, d);
        }
    }
}

// Inserts a new key into the B-tree. If a leaf node becomes full after insertion,
// it triggers a split.
void create(bdnode *&T, int num, int d) {
    if (T == NULL) {
        T = new bdnode(d);
        T->key[0] = num;
        T->count = 1;
        return;
    }

    // Traverse down the tree to find the appropriate leaf node
    bdnode *cur = T;
    while (cur->ptr[0] != NULL) {
        int i = 0;
        while (i < cur->count && cur->key[i] < num)
            i++;
        cur = cur->ptr[i];
    }

    // Insert key in sorted order into the leaf node
    insertSorted(cur->key, cur->count, num);
    // If node is full, split it
    if (cur->count == d) {
        splitNode(T, cur, d);
    }
}
void inorder(bdnode* T){
    if(T==NULL){
        return;
    }
    for(int i=0;i<T->count;i++){
        inorder(T->ptr[i]);
        cout<<T->key[i]<<" ";
    }
    inorder(T->ptr[T->count]);
}
// Prints the B-tree using level order traversal.
// Each level is separated by printing "-1".
void levelOrder(bdnode *T) {
    if (!T)
        return;
    queue<bdnode *> q;
    q.push(T);

    while (!q.empty()) {
        int size = q.size();
        while (size--) {
            bdnode *temp = q.front(); 
            q.pop();
            for (int i = 0; i < temp->count; i++) {
                cout << temp->key[i] << " ";
                if (temp->ptr[i])
                    q.push(temp->ptr[i]);
            }
            if (temp->ptr[temp->count])
                q.push(temp->ptr[temp->count]);
        }
        cout << "-1" << endl;
    }
}

int main() {
    bdnode *T = NULL;
    int d;cin>>d;  // Maximum capacity (number of keys allowed per node) is d (here, 3)
    int num;
    // Read integers until -1 is entered.
    while (cin >> num && num != -1) {
        create(T, num, d);
    }
    inorder(T);
    cout<<endl;
    levelOrder(T);
    return 0;
}
