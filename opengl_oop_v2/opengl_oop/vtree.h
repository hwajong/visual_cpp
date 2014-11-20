#ifndef __VTREE_H__
#define __VTREE_H__

#include <vector>
#include "vobject.h"

using namespace std;

// ��� ����
struct node
{
	int id;
	VObject* vobj;
	vector<node*> children;

	node(int id, VObject* vobj) : id(id), vobj(vobj)
	{
	}

	~node()
	{
		if (vobj) delete vobj;
	}
};

// Ʈ�� Ŭ���� ����
class VTree
{
public:

	VTree() : root(NULL), node_size(0)
	{
	}

	void draw()
	{
		if (root == NULL) return;

		// ��Ŀ�ú��ϰ� ĥ�己 ��ü�� ����� vobj �� draw �Ѵ�.
		int size = root->children.size();
		for (int i = 0; i < size; i++)
		{
			_draw((node*)root->children[i]);
		}
	}

	void add(VObject* vobj)
	{
		if (root == NULL)
		{
			// TODO ��Ʈ���� ������ ��ü 
			root = new node(0, NULL);
			printf("*root node created\n");
		}

		// �߰��Ǵ� ��ü�� child �Ǵ� sibling ��ü�� �߰��Ѵ�.
		_add(root, vobj);
	}

	// ��� ��� ����
	void clear()
	{
		_clear(root);
	}

	// p1, p2 �� �簢 ���� ���� ��� ��ü�� �����Ѵ�.
	void remove(Point p1, Point p2)
	{
		if (root == NULL) return;

		Rectangle rect(p1, p2);

		vector<node*>::iterator i = root->children.begin();

		while (i != root->children.end())
		{
			if (rect.is_include_area((*i)->vobj))
			{
				printf("*erasing ... %d node(%s)\n", (*i)->id, (*i)->vobj->name().c_str());
				_clear(*i);
				i = root->children.erase(i);
				continue;
			}

			i++;
		}

		printf("*current tree node size : %d\n", node_size);
	}

private:

	node* root;
	int node_size;

	void _draw(node* n)
	{
		if (n == NULL) return;

		n->vobj->draw();
		int size = n->children.size();
		for (int i = 0; i < size; i++)
		{
			_draw((node*)n->children[i]);
		}
	}

	void _add(node* cur, VObject* vobj)
	{
		int size = cur->children.size();
		for (int i = 0; i < size; i++)
		{
			node* n = (node*)cur->children[i];
			if (n->vobj->is_include_area(vobj))
			{
				printf("*%d node(%s) is child of %d node(%s)\n",
					node_size + 1, vobj->name().c_str(), n->id, n->vobj->name().c_str());
				_add(n, vobj);
				return;
			}
		}

		node_size++;
		printf("*%d node created - %s\n", node_size, vobj->name().c_str());
		cur->children.push_back(new node(node_size, vobj));
	}

	void _clear(node* n)
	{
		if (n == NULL) return;

		int size = n->children.size();
		for (int i = 0; i < size; i++)
		{
			_clear((node*)n->children[i]);
		}

		node_size--;
		delete n;
	}
};


#endif