/**
 *   ///////////////////
 *   // Dancing Links //
 *   ///////////////////
 *
 * A doubly linked list implementation that allows re-insertion
 * of deleted elements in Last-Out-First-In order. Based on
 * Knuth's dancing links.
 *
 
#include <stdlib.h>

template< class T >
struct Node
{
    T v;
    Node *l, *r;

    Node( const T &x, Node *a = NULL, Node *b = NULL ) : v( x ), l( a ), r( b ) {}
};

template< class T >
class DList
{
  public:

    Node< T > *first, *last;
    int size;

    DList() { first = last = NULL; size = 0; }
    ~DList() { while( last ) { Node< T > *p = last->l; delete last; last = p; } }

    void push_front( const T &x )
    {
        first = new Node< T >( x, NULL, first );
        if( first->r ) first->r->l = first;
        else last = first;
        size++;
    }

    void push_back( const T &x )
    {
        last = new Node< T >( x, last );
        if( last->l ) last->l->r = last;
        else first = last;
        size++;
    }

    void erase( Node< T > *x )
    {
        if( x->l ) x->l->r = x->r; else first = x->r;
        if( x->r ) x->r->l = x->l; else last = x->l;
        size--;
    }

    void pop_front() { erase( first ); }
    void pop_back() { erase( last ); }

    /** Takes a recently erased element and restores it. **/
    void restore( Node< T > *x )
    {
        if( x->l ) x->l->r = x; else first = x;
        if( x->r ) x->r->l = x; else last = x;
        size++;
    }
};

//DEBUG
// The rest of the code generates permutations in lex order
#include <stdio.h>
void boogie( DList< int > &dl, DList< int > &ans );
int main()
{
    // generate all permutations of 4 elements in lex order
    DList< int > dl, ans;
    for( int i = 0; i < 4; i++ ) dl.push_back( i );
    boogie( dl, ans );
    return 0;
}
void boogie( DList< int > &dl, DList< int > &ans )
{
    if( !dl.size )
    {
        for( Node< int > *i = ans.first; i; i = i->r ) printf( " %d", i->v );
        puts( "" );
        return;
    }

    Node< int > *i = dl.first;
    while( i )
    {
        dl.erase( i );
        ans.push_back( i->v );
        boogie( dl, ans );
        ans.pop_back();
        dl.restore( i );
        i = i->r;
    }
}
