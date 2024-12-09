//-----------------------------------------------------------------------------
// author Lyp
// date 2011-05-04
// last 2011-05-04
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 红黑树简单类型map
//-----------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
class EMap
{
public:
	enum nodecolor { BLACK, RED };
	struct tagNode
	{
		tagNode*	left;
		tagNode*	right;
		tagNode*	up;
		nodecolor	color;
		KEY_TYPE	key;
		VALUE_TYPE	value;
	};


	//-------------------------------------------------------------------------------
	// 添加元素
	//-------------------------------------------------------------------------------
	BOOL Add(KEY_TYPE key, VALUE_TYPE value)
	{
		tagNode *x = rb_insert(key, value);
		return (x!=&rb_null) ? TRUE : FALSE;
	}

	//------------------------------------------------------------------------
	// 改变指定key对应的值
	//------------------------------------------------------------------------
	BOOL ChangeValue(KEY_TYPE key, VALUE_TYPE new_value)
	{
		if(rb_root==NULL)	// empty tree
			return FALSE;

		tagNode *x = rb_lookup(key);
		if(x==&rb_null)
			return FALSE;
		
		x->value = new_value;
		return TRUE;
	}

	//-------------------------------------------------------------------------------
	// 根据键得到值
	//-------------------------------------------------------------------------------
	VALUE_TYPE Peek(KEY_TYPE key)
	{
		if(rb_root==NULL)	// empty tree
			return VALUE_TYPE(INVALID);

		tagNode *x = rb_lookup(key);
		return((x==&rb_null) ? VALUE_TYPE(INVALID) : x->value);
	}

	//-------------------------------------------------------------------------------
	// 方括号操作符
	//-------------------------------------------------------------------------------
	__forceinline VALUE_TYPE& operator[](KEY_TYPE key)
	{
		if(rb_root==NULL)	// empty tree
			return VALUE_TYPE(INVALID);

		tagNode *x = rb_lookup(key);
		return((x==&rb_null) ? VALUE_TYPE(INVALID) : x->value);
	}


	//-------------------------------------------------------------------------------
	// 删除元素
	//-------------------------------------------------------------------------------
	BOOL Erase(KEY_TYPE key)
	{
		tagNode *x = rb_lookup(key);
		if(x==&rb_null)
			return FALSE;

		rb_delete(x);
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 检查指定元素是否存在
	//-------------------------------------------------------------------------
	BOOL IsExist(KEY_TYPE key)
	{
		tagNode *x = rb_lookup(key);
		if(x!=&rb_null)
			return TRUE;

		return FALSE;
	}

	//-------------------------------------------------------------------------------
	// 重置游标
	//-------------------------------------------------------------------------------
	void ResetIterator()
	{
		rb_current = rb_root;
		if(rb_current!=&rb_null)
		{
			while(rb_current->left!=&rb_null)
				rb_current=rb_current->left;
		}
	}

	//-------------------------------------------------------------------------------
	// 获取下个元素
	//-------------------------------------------------------------------------------
	BOOL PeekNext(VALUE_TYPE& value)
	{
		if(rb_current==&rb_null)
			return FALSE;
		
		value=rb_current->value;
		rb_current=rb_successor(rb_current);
		return TRUE;
	}
	
	BOOL PeekNext(KEY_TYPE& key, VALUE_TYPE& value)
	{
		if(rb_current==&rb_null)
			return FALSE;
		
		key = rb_current->key;
		value = rb_current->value;
		rb_current = rb_successor(rb_current);
		return TRUE;
	}

	//-------------------------------------------------------------------------------
	// 得到元素个数
	//-------------------------------------------------------------------------------
	SIZE_T	Size()	{ return m_stItem; }
	BOOL	Empty() { return 0 == m_stItem; }

	//-------------------------------------------------------------------------------
	// 清除所有元素
	//-------------------------------------------------------------------------------
	VOID Clear() { rb_destroy(rb_root); rb_current = &rb_null; rb_root = &rb_null; }


	//--------------------------------------------------------------------------
	// 将所有key导出到一个给定std::list
	//--------------------------------------------------------------------------
	VOID ExportAllKey(std::list<KEY_TYPE>& listDest) const
	{
		tagNode* rb_node = rb_root;
		if(rb_node!=&rb_null)
		{
			while(rb_node->left!=&rb_null)
				rb_node=rb_node->left;
		}
		while(rb_node!=&rb_null)
		{
			listDest.push_back(rb_node->key);
			rb_node=rb_successor(rb_node);
		}
	}
	
	//--------------------------------------------------------------------------
	// 将所有value导出到一个给定std::list
	//--------------------------------------------------------------------------
	VOID ExportAllValue(std::list<VALUE_TYPE>& listDest) const
	{
		tagNode* rb_node = rb_root;
		if(rb_node!=&rb_null)
		{
			while(rb_node->left!=&rb_null)
				rb_node=rb_node->left;
		}
		while(rb_node!=&rb_null)
		{
			listDest.push_back(rb_node->value);
			rb_node=rb_successor(rb_node);
		}
	}



	EMap():rb_root(&rb_null), rb_current(&rb_null), m_stItem(0)
	{
		rb_null.left = &rb_null;
		rb_null.right = &rb_null;
		rb_null.up = &rb_null;
		rb_null.color = BLACK;
	}

	~EMap() { Clear(); }

private:
	EMap( EMap<KEY_TYPE, VALUE_TYPE>& );				// 不允许拷贝
	void operator=( EMap<KEY_TYPE, VALUE_TYPE>& );		// 不允许拷贝

	tagNode rb_null;	// dummy null
	tagNode* rb_root;
	tagNode* rb_current;
	SIZE_T m_stItem;


	// Destroy all the elements blow us in the tree only useful as part of a complete tree destroy.
	__forceinline void rb_destroy(tagNode *x)
	{
		if (x!=&rb_null)
		{
			if (x->left!=&rb_null)
				rb_destroy(x->left);
			if (x->right!=&rb_null)
				rb_destroy(x->right);
			//MCFREE(x);
#ifdef __SGI_STL_PORT
			__sgi_alloc::deallocate(x, sizeof(tagNode));
#else
			free(x);
#endif
			
			--m_stItem;
		}
	}

	// Search for and if not found and insert is true, will add a new node in. Returns a pointer to the new node, or the node found
	__forceinline tagNode* rb_insert(KEY_TYPE key, VALUE_TYPE value)
	{
		tagNode *x=rb_root, *y=&rb_null;

		/* walk x down the tree */
		while( x!=&rb_null )
		{
			y=x;
			if(key < x->key)
				x=x->left;
			else if(key > x->key)
				x=x->right;
			else
				return &rb_null;	// 有重复的key
		}

		size_t dwSize = sizeof(tagNode);
		//tagNode* z = (tagNode*)MCALLOC(dwSize);

#ifdef __SGI_STL_PORT
		tagNode* z = (tagNode*)__sgi_alloc::allocate(dwSize); 
#else
		tagNode* z = (tagNode*)malloc(dwSize); 
#endif

		++m_stItem;
		z->key = key;
		z->value = value;
		z->up=y;
		if( y==&rb_null )
		{
			rb_root=z;
		}
		else
		{
			if(z->key < y->key)
				y->left=z;
			else
				y->right=z;
		}

		z->left=&rb_null;
		z->right=&rb_null;

		// color this new node red
		z->color=RED;

		// Having added a red node, we must now walk back up the tree balancing
		// it, by a series of rotations and changing of colours
		x=z;

		// While we are not at the top and our parent node is red. N.B. Since the root node is garanteed black, then we
		// are also going to stop if we are the child of the root
		while(x != rb_root && (x->up->color == RED))
		{
			// if our parent is on the left side of our grandparent
			if (x->up == x->up->up->left)
			{
				y=x->up->up->right;	// get the right side of our grandparent (uncle?)
				if (y->color == RED)
				{
					x->up->color = BLACK;	// make our parent black
					y->color = BLACK;	// make our uncle black
					x->up->up->color = RED;	// make our grandparent red
					x=x->up->up;	// now consider our grandparent
				}
				else
				{
					// if we are on the right side of our parent
					if (x == x->up->right)
					{
						x=x->up;
						rb_left_rotate(x);
					}

					x->up->color = BLACK;	// make our parent black
					x->up->up->color = RED;	// make our grandparent red
					rb_right_rotate(x->up->up);	// right rotate our grandparent
				}
			}
			else
			{
				// everything here is the same as above, but exchanging left for right
				y=x->up->up->left;
				if (y->color == RED)
				{
					x->up->color = BLACK;
					y->color = BLACK;
					x->up->up->color = RED;
					x=x->up->up;
				}
				else
				{
					if (x == x->up->left)
					{
						x=x->up;
						rb_right_rotate(x);
					}

					x->up->color = BLACK;
					x->up->up->color = RED;
					rb_left_rotate(x->up->up);
				}
			}
		}

		rb_root->color = BLACK;	// Set the root node black
		return(z);
	}


	// Search for a key according to mode (see redblack.h)
	__forceinline tagNode* rb_lookup(KEY_TYPE key)
	{
		tagNode *x=rb_root;

		// walk x down the tree
		while(x!=&rb_null)
		{
			if(key < x->key)
				x=x->left;
			else if(key > x->key)
				x=x->right;
			else
				return(x);
		}

		return(&rb_null);
	}


	__forceinline void rb_left_rotate(tagNode *x)
	{
		tagNode *t = x->right;
		x->right = t->left;

		if (t->left != &rb_null)
			t->left->up = x;

		t->up = x->up;

		if (x->up == &rb_null)
			rb_root=t;
		else
		{
			if( x == x->up->left )
				x->up->left=t;
			else
				x->up->right=t;
		}

		t->left=x;
		x->up = t;
	}


	__forceinline void rb_right_rotate(tagNode *y)
	{
		tagNode *t = y->left;
		y->left = t->right;

		if (t->right != &rb_null)
			t->right->up = y;

		t->up = y->up;

		if (y->up == &rb_null)
			rb_root=t;
		else
		{
			if (y == y->up->left)
				y->up->left=t;
			else
				y->up->right=t;
		}

		t->right=y;
		y->up = t;
	}

	// Return a pointer to the smallest key greater than x
	__forceinline tagNode* rb_successor(const tagNode *x) const
	{
		tagNode* t;
		if (x->right!=&rb_null)
			for( t=x->right; t->left!=&rb_null; t=t->left );
		else
		{ t=x->up; while(t!=&rb_null && x==t->right) { x=t; t=t->up; }	}
		return(t);
	}

	// Return a pointer to the largest key smaller than x
	__forceinline tagNode* rb_predecessor(const tagNode *x)
	{
		tagNode* t;
		if( x->left!=&rb_null )
			for( t=x->left; t->right!=&rb_null; t=t->right );
		else
		{ t=x->up; while(t!=&rb_null && x==t->left) { x=t; t=t->up; } }
		return(t);
	}

	// Delete the node z, and free up the space
	__forceinline void rb_delete(tagNode *z)
	{
		tagNode *x, *y;

		if (z->left == &rb_null || z->right == &rb_null)
			y=z;
		else
			y=rb_successor(z);

		if (y->left != &rb_null)
			x=y->left;
		else
			x=y->right;

		x->up = y->up;

		if (y->up == &rb_null)
		{
			rb_root=x;
		}
		else
		{
			if (y==y->up->left)
				y->up->left = x;
			else
				y->up->right = x;
		}

		if (y!=z)
		{
			z->key = y->key;
			z->value = y->value;
		}

		if (y->color == BLACK)	// Restore the reb-black properties after a delete
		{
			tagNode *w;
			while (x!=rb_root && x->color==BLACK)
			{
				if (x==x->up->left)
				{
					w=x->up->right;
					if (w->color==RED)
					{
						w->color=BLACK;
						x->up->color=RED;
						rb_left_rotate(x->up);
						w=x->up->right;
					}

					if (w->left->color==BLACK && w->right->color==BLACK)
					{
						w->color=RED;
						x=x->up;
					}
					else
					{
						if (w->right->color == BLACK)
						{
							w->left->color=BLACK;
							w->color=RED;
							rb_right_rotate(w);
							w=x->up->right;
						}

						w->color=x->up->color;
						x->up->color = BLACK;
						w->right->color = BLACK;
						rb_left_rotate(x->up);
						x=rb_root;
					}
				}
				else
				{
					w=x->up->left;
					if (w->color==RED)
					{
						w->color=BLACK;
						x->up->color=RED;
						rb_right_rotate(x->up);
						w=x->up->left;
					}

					if (w->right->color==BLACK && w->left->color==BLACK)
					{
						w->color=RED;
						x=x->up;
					}
					else
					{
						if (w->left->color == BLACK)
						{
							w->right->color=BLACK;
							w->color=RED;
							rb_left_rotate(w);
							w=x->up->left;
						}

						w->color=x->up->color;
						x->up->color = BLACK;
						w->left->color = BLACK;
						rb_right_rotate(x->up);
						x=rb_root;
					}
				}
			}

			x->color=BLACK;
		}

		//MCFREE(y);
#ifdef __SGI_STL_PORT
		__sgi_alloc::deallocate(y, sizeof(tagNode));
#else
		free(y);
#endif

		--m_stItem;
	}
};
