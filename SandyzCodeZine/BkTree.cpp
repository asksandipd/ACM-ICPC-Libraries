/**
 * Burkhard-Keller Tree 
 *
 * Maintains a finite metric space with an integer-valued distance function.
 * Supports insert() and getWithinDistance(k).
 *
 * TEMPLATE PARAMETERS:
 *   T: element type
 *   Distance: a function (or functor) from TxT to non-negative integers.
 *       Must be a metric. Distance zero implies equality.
 *
 * FIELD TESTING:
 *     None; use at your own risk.
 *
  
#include <assert.h>
#include <queue>
using namespace std;

template<class T, int (*Distance)( const T&, const T& )>
class BkTree
{
 public:
  BkTree() { root_ = NULL; size_ = 0; }

  ~BkTree() { if( root_ ) delete root_; }

  /** Does nothing if the tree already contains this element. **/
  void insert(T item)
  {
    if( !root_ )
    {
      size_ = 1;
      root_ = new Node(item, -1);
      return;
    }

    Node *t = root_;
    while( true )
    {
      int d = Distance( t->item, item );
      if( !d ) return;
      Node *ch = t->firstChild;
      while( ch )
      {
        if( ch->distToParent == d ) { t = ch; break; }
        ch = ch->nextSibling;
      }
      if( !ch )
      {
        Node *newChild = new Node(item, d);
        newChild->nextSibling = t->firstChild;
        t->firstChild = newChild;
        size_++;
        break;
      }
    }
  }

  int size() const { return size_; }
  
  bool empty() const { return !size_; }

  int count( T item ) const { return getWithinDistance( item, 0 ); }

  /**
   * Finds all elements within a distance of 'k' of 'center', inclusive.
   * Fills 'result' and returns the number of elements found.
   * Gives no guarantees about the order in which results are returned.
   **/
  int getWithinDistance(T center, int k, vector<T> *result = NULL) const
  {
    assert( k >= 0 );
    if( !root_ ) return 0;

    int found = 0;
    queue< Node* > q;
    q.push( root_ );

    while( !q.empty() )
    {
      Node *t = q.front(); q.pop();
      int d = Distance( t->item, center );
      if( d <= k ) { if( result ) result->push_back( t->item ); found++; }

      Node *ch = t->firstChild;
      while( ch )
      {
        if( d - k <= ch->distToParent && ch->distToParent <= d + k )
          q.push(ch);
        ch = ch->nextSibling;
      }
    }

    return found;
  }

 private:
  struct Node
  {
    T item;
    int distToParent;
    Node *firstChild;
    Node *nextSibling;

    Node(T x, int dist)
    {
      item = x;
      distToParent = dist;
      firstChild = nextSibling = NULL;
    }

    ~Node()
    {
      if( firstChild ) delete firstChild;
      if( nextSibling ) delete nextSibling;
    }
  };

  Node *root_;
  int size_;
};


//------------- TESTING --------------
#include <stdio.h>
#include <string>

int EditDistance( const string &s, const string &t )
{
  int m = s.size();
  int n = t.size();
  vector< vector< int > > tab( m + 1, vector< int >( n + 1, 0 ) );
  for( int i = m - 1; i >= 0; i-- ) for( int j = n - 1; j >= 0; j-- )
  {
    if( s[i] == t[j] ) tab[i][j] = tab[i + 1][j + 1];
    else tab[i][j] = 1 + (tab[i + 1][j] <? tab[i][j] <? tab[i][j + 1]);
  }
  return tab[0][0];
}

int main()
{
  BkTree<string, &EditDistance> tree;
  assert( tree.size() == 0 );
  assert( tree.empty() );

  tree.insert( "boobs" );
  assert( tree.size() == 1 );
  assert( !tree.empty() );
  assert( tree.count( "boobs" ) );
  assert( !tree.count( "books" ) );
  assert( tree.getWithinDistance( "boobs", 0 ) == 1 );
  assert( tree.getWithinDistance( "boobs", 1 ) == 1 );
  assert( tree.getWithinDistance( "books", 0 ) == 0 );
  assert( tree.getWithinDistance( "books", 1 ) == 1 );

  tree.insert( "books" );
  assert( tree.size() == 2 );
  assert( !tree.empty() );
  assert( tree.count( "boobs" ) );
  assert( tree.count( "books" ) );
  assert( !tree.count( "boots" ) );
  assert( tree.getWithinDistance( "books", 0 ) == 1 );
  assert( tree.getWithinDistance( "books", 1 ) == 2 );
  assert( tree.getWithinDistance( "boots", 1 ) == 2 );
  assert( tree.getWithinDistance( "boobs", 1 ) == 2 );

  vector< string > pool;
  int n = 100;
  int len = 10;
  for( int i = 0; i < n; i++ )
  {
    string word;
    while( word.size() < len ) word += 'a' + rand() % 26;
    pool.push_back( word );
  }

  vector< vector< int > > dist( n, vector< int >( n ) );
  for( int i = 0; i < n; i++ ) for( int j = 0; j < n; j++ )
    if( i == j )
      dist[i][j] = 0;
    else
      assert( dist[i][j] = EditDistance( pool[i], pool[j] ) );

  BkTree< string, EditDistance > tree2;
  for( int i = 0; i < n; i++ )
  {
    assert( tree2.size() == i );
    assert( !tree2.count( pool[i] ) );
    tree2.insert( pool[i] );
    assert( tree2.count( pool[i] ) );
    for( int d = 0; d <= len; d++ )
    {
      int ans = 0;
      for( int j = 0; j <= i; j++ ) if( dist[i][j] <= d ) ans++;
      assert( tree2.getWithinDistance( pool[i], d ) == ans );
    }
  }
  assert( tree2.size() == n );

  puts( "Oll Korrekt." );
}

