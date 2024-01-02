/*!
    \file "main.cpp"

    Author: Matt Ervin <matt@impsoftware.org>
    Formatting: 4 spaces/tab (spaces only; no tabs), 120 columns.
    Doc-tool: Doxygen (http://www.doxygen.com/)

    https://leetcode.com/problems/delete-node-in-a-bst/description/
*/

//!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <algorithm>
#include <cassert>
#include <chrono>
#include <doctest/doctest.h> //!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/tutorial.md
#include <iterator>
#include <memory>
#include <queue>
#include <vector>
#include <set>
#include <span>

using namespace std;

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
    virtual ~TreeNode() {
        if (left) {
            delete left;
        }
        if (right) {
            delete right;
        }
    }
};

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    TreeNode* findMinValNode(TreeNode* root) {
        while (root && root->left) {
            root = root->left;
        }
        return root;
    }

    /*
        Simple maintainable solution:

        if !root:
            return nil
        
        if key < root->val:
            root->left = deleteNode(root->left, key)
        else if key > root->val:
            root->right = deleteNode(root->right, key)
        else: # Value found.
            if !root->left:
                tmp = root->right
                root->right = nil
                delete root
                return tmp
            else if !root->right:
                tmp = root->left
                root->left = nil
                delete root
                return tmp
            else:
                minNode = findMinValueNode(root->right)
                root->val = move(minNode->val)
                root->right = deleteNode(root->right, minNode->val) # Causes findMinValueNode() to be called twice!
        
        return root
    
        Time = O(2*log2(n)) => O(log2(n))
               o Recursive calls = log2(n)
               o Find minimum value node = log2(n)
        Space = O(log2(n))  [for call stack]
    */
    TreeNode* deleteNode(TreeNode* root, int const key) {
        if (root) {
            if (key < root->val) {
                root->left = deleteNode(root->left, std::move(key));
            } else if (key == root->val) {
                if (!root->left) {
                    auto rootRaii = std::unique_ptr<TreeNode>{root};
                    auto child = root->right;
                    root->right = nullptr;
                    root = child;
                } else if (!root->right) {
                    auto rootRaii = std::unique_ptr<TreeNode>{root};
                    auto child = root->left;
                    root->left = nullptr;
                    root = child;
                } else {
                    auto minValNode = findMinValNode(root->right);
                    root->val = std::move(minValNode->val);
                    root->right = deleteNode(root->right, minValNode->val);
                }
            } else {
                root->right = deleteNode(root->right, std::move(key));
            }
        }

        return root;
    }
};

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

struct elapsed_time_t
{
    std::chrono::steady_clock::time_point start{};
    std::chrono::steady_clock::time_point end{};
    
    elapsed_time_t(
        std::chrono::steady_clock::time_point start
        , std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now()
    ) : start{std::move(start)}, end{std::move(end)} {}
};

std::ostream&
operator<<(std::ostream& os, elapsed_time_t const& et)
{
    auto const elapsed_time = et.end - et.start;
    os << std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count()
       << '.' << (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count() % 1000)
       << " ms";
    return os;
}

TreeNode* createBst(std::vector<int> values) {
    TreeNode* root = nullptr;

    for (auto value : values) {
        // Search for parent node, if any.
        TreeNode* parent{};
        for (TreeNode* iter = root; iter; ) {
            parent = iter;
            iter = value < iter->val ? iter->left : iter->right;
        }
        
        // Find parent pointer.
        TreeNode** parentPtr{};
        if (parent) {
            if (value < parent->val) {
                parentPtr = &parent->left;
            } else {
                parentPtr = &parent->right;
            }
        } else {
            parentPtr = &root;
        }
        
        // Allocate new node.
        if (parentPtr) {
            *parentPtr = new TreeNode{std::move(value)};

            if (!root) {
                root = *parentPtr;
            }
        }
    }

    return root;
}

std::vector<int> toVector(TreeNode const* root) {
    std::vector<int> result{};

    // In order depth first search.
    if (root) {
        auto const leftResult = toVector(root->left);
        std::copy(leftResult.begin(), leftResult.end(), std::back_inserter(result));

        result.push_back(root->val);

        auto const rightResult = toVector(root->right);
        std::copy(rightResult.begin(), rightResult.end(), std::back_inserter(result));
    }

    return result;
}

TEST_CASE("Case 1")
{
    cerr << "Case 1" << '\n';
    std::vector<int> const values{5,3,6,2,4,7};
    auto root = std::unique_ptr<TreeNode>{createBst(values)};
    auto const removeValue = 3;
    auto const expected = [&]{
        auto result{values};
        auto found = std::find(result.begin(), result.end(), removeValue);
        if (result.end() != found) { result.erase(found); }
        std::sort(result.begin(), result.end());
        return result;
    }();
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        root.reset(Solution{}.deleteNode(root.release(), removeValue));
        auto const result = toVector(root.get());
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 2")
{
    cerr << "Case 2" << '\n';
    std::vector<int> const values{5,3,6,2,4,7};
    auto root = std::unique_ptr<TreeNode>{createBst(values)};
    auto const removeValue = 0;
    auto const expected = [&]{
        auto result{values};
        auto found = std::find(result.begin(), result.end(), removeValue);
        if (result.end() != found) { result.erase(found); }
        std::sort(result.begin(), result.end());
        return result;
    }();
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        root.reset(Solution{}.deleteNode(root.release(), removeValue));
        auto const result = toVector(root.get());
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 3")
{
    cerr << "Case 3" << '\n';
    std::vector<int> const values{};
    auto root = std::unique_ptr<TreeNode>{createBst(values)};
    auto const removeValue = 0;
    auto const expected = [&]{
        auto result{values};
        auto found = std::find(result.begin(), result.end(), removeValue);
        if (result.end() != found) { result.erase(found); }
        std::sort(result.begin(), result.end());
        return result;
    }();
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        root.reset(Solution{}.deleteNode(root.release(), removeValue));
        auto const result = toVector(root.get());
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 4")
{
    cerr << "Case 4" << '\n';
    std::vector<int> const values{5,3,6,2,4,7};
    auto root = std::unique_ptr<TreeNode>{createBst(values)};
    auto const removeValue = 5;
    auto const expected = [&]{
        auto result{values};
        auto found = std::find(result.begin(), result.end(), removeValue);
        if (result.end() != found) { result.erase(found); }
        std::sort(result.begin(), result.end());
        return result;
    }();
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        root.reset(Solution{}.deleteNode(root.release(), removeValue));
        auto const result = toVector(root.get());
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 5")
{
    cerr << "Case 5" << '\n';
    std::vector<int> const values{50,30,70,40,60,80};
    auto root = std::unique_ptr<TreeNode>{createBst(values)};
    auto const removeValue = 50;
    auto const expected = [&]{
        auto result{values};
        auto found = std::find(result.begin(), result.end(), removeValue);
        if (result.end() != found) { result.erase(found); }
        std::sort(result.begin(), result.end());
        return result;
    }();
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        root.reset(Solution{}.deleteNode(root.release(), removeValue));
        auto const result = toVector(root.get());
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

/*
    End of "main.cpp"
*/
