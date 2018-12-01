#include <boost/scoped_ptr.hpp>
#include <cassert>
#include "Widget.h"

void useTwoWidgets() {
    // default constructed scoped_ptr
    boost::scoped_ptr<Widget> wgt;
    assert(!wgt);               // null test - Boolean context

    wgt.reset(new Widget);      // create first widget
    assert(wgt);                // non-null test – Boolean context
    wgt->display();             // display first widget
    wgt.reset(new Widget);      // destroy first, create second widget
    wgt->display();             // display second widget

    Widget *w1 = wgt.get();     // get the raw pointer
    Widget &rw1 = *wgt;         // 'dereference' the smart pointer
    assert(w1 == &rw1);         // same object, so same address

    boost::scoped_ptr<Widget> wgt2(new Widget);
    Widget *w2 = wgt2.get();
    wgt.swap(wgt2);
    assert(wgt.get() == w2);    // effect of swap
    assert(wgt2.get() == w1);   // effect of swap
}

int main() {
    useTwoWidgets();
}

/**
 * 主要特点：（1）scoped_ptr 只限于作用域内使用；（2）指针管理权不可转移，不支持拷贝构造函数与赋值操作。
 *
 * 这种智能指针只限于作用域内使用，无法转移内置指针的管理权(不支持拷贝、=赋值等) 但是作用也很显然，例如：
 *   void test()
 *  {
 *       int* p = new int(3);
 *       ...
 *       delete p;
 *   }
 * 假设定义到delete之中…发生了异常，那么p就无法被delete，造成了内存泄漏。使用scoped_ptr就可以很好解决这
 * 个问题，只需要new的时候放到scoped_ptr之中就可以。示例如下：
 *  // scoped_ptr usage
 *  scoped_ptr<string> sp(new string("text"));
 *  cout << *sp << endl;
 *  cout << sp->size() << endl;
 *
 *  // pointer 管理权移交 scoped_ptr
 *  auto_ptr<int> ap(new int(10));
 *  scoped_ptr<int> scoped(ap);
 *  assert(ap.get() == 0);
 *
 *  ap.reset(new int(20));
 *  cout << *ap << ", " << *scoped << endl;
 *
 *  auto_ptr<int> ap2;
 *  ap2 = ap;
 *  assert(ap.get() == 0);			// ap is null-pointer
 *
 */
