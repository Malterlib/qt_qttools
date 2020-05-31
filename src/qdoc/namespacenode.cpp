/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "namespacenode.h"

#include "codeparser.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

/*!
  \class NamespaceNode
  \brief This class represents a C++ namespace.

  A namespace can be used in multiple C++ modules, so there
  can be a NamespaceNode for namespace Xxx in more than one
  Node tree.
 */

/*! \fn NamespaceNode(Aggregate *parent, const QString &name)
  Constructs a NamespaceNode with the specified \a parent and \a name.
  The node type is Node::Namespace.
 */

/*!
  The destructor removes all children from the child list that
  have a parent() that is not this NamespaceNode. This situation
  can arise because of elements that are related to this namespace
  using the \c {\\relates} command.

  \note The child nodes remaining in the child list after the ones
  with a different parent() have been removed are deleted in the
  destructor of the Aggregate base class.
 */
NamespaceNode::~NamespaceNode()
{
    for (int i = 0; i < children_.size(); ++i) {
        if (children_[i]->parent() != this)
            children_[i] = nullptr;
    }
}

/*!
  Returns true if this namespace is to be documented in the
  current module. There can be elements declared in this
  namespace spread over multiple modules. Those elements are
  documented in the modules where they are declared, but they
  are linked to from the namespace page in the module where
  the namespace itself is documented.
 */
bool NamespaceNode::isDocumentedHere() const
{
    return m_whereDocumented == tree()->camelCaseModuleName();
}

/*!
  Returns true if this namespace node contains at least one
  child that has documentation and is not private or internal.
 */
bool NamespaceNode::hasDocumentedChildren() const
{
    for (const auto *node : qAsConst(children_)) {
        if (node->isInAPI())
            return true;
    }
    return false;
}

/*!
  Report qdoc warning for each documented child in a namespace
  that is not documented. This function should only be called
  when the namespace is not documented.
 */
void NamespaceNode::reportDocumentedChildrenInUndocumentedNamespace() const
{
    for (const auto *node : qAsConst(children_)) {
        if (node->isInAPI()) {
            QString msg1 = node->name();
            if (node->isFunction())
                msg1 += "()";
            msg1 += QStringLiteral(
                            " is documented, but namespace %1 is not documented in any module.")
                            .arg(name());
            QString msg2 =
                    QStringLiteral(
                            "Add /*! '\\%1 %2' ... */ or remove the qdoc comment marker (!) at "
                            "that line number.")
                            .arg(COMMAND_NAMESPACE)
                            .arg(name());

            node->doc().location().warning(msg1, msg2);
        }
    }
}

/*!
  Returns true if this namespace node is not private and
  contains at least one public child node with documentation.
 */
bool NamespaceNode::docMustBeGenerated() const
{
    if (isInAPI())
        return true;
    return hasDocumentedChildren();
}

/*!
  Returns a const reference to the namespace node's list of
  included children, which contains pointers to all the child
  nodes of other namespace nodes that have the same name as
  this namespace node. The list is built after the prepare
  phase has been run but just before the generate phase. It
  is buils by QDocDatabase::resolveNamespaces().

  \sa QDocDatabase::resolveNamespaces()
 */
const NodeList &NamespaceNode::includedChildren() const
{
    return m_includedChildren;
}

/*!
  This function is only called from QDocDatabase::resolveNamesapces().

  \sa includedChildren(), QDocDatabase::resolveNamespaces()
 */
void NamespaceNode::includeChild(Node *child)
{
    m_includedChildren.append(child);
}

/*! \fn Tree* NamespaceNode::tree() const
  Returns a pointer to the Tree that contains this NamespaceNode.
  This requires traversing the parent() pointers to the root of
  the Tree, which is the unnamed NamespaceNode.
 */

/*! \fn bool NamespaceNode::isFirstClassAggregate() const
  Returns \c true.
 */

/*! \fn bool NamespaceNode::isRelatableType() const
  Returns \c true.
 */

/*! \fn bool NamespaceNode::wasSeen() const
  Returns \c true if the \c {\\namespace} command that this NamespaceNode
  represents has been parsed by qdoc. When \c false is returned, it means
  that only \c {\\relates} commands have been seen that relate elements to
  this namespace.
 */

/*! \fn void NamespaceNode::markSeen()
  Sets the data member that indicates that the \c {\\namespace} command this
  NamespaceNode represents has been parsed by qdoc.
 */

/*! \fn void NamespaceNode::markNotSeen()
  Clears the data member that indicates that the \c {\\namespace} command this
  NamespaceNode represents has been parsed by qdoc.
 */

/*! \fn void NamespaceNode::setTree(Tree* t)
  Sets the Tree pointer to \a t, which means this NamespaceNode is in the Tree \a t.
 */

/*! \fn QString NamespaceNode::whereDocumented() const
  Returns the camel case name of the module where this namespace is documented.

  \sa setWhereDocumented()
 */

/*! \fn void NamespaceNode::setWhereDocumented(const QString &t)
  Sets the camel case name of the module where this namespace is documented to
  the module named \a t.

  This function is called when the \c {\\namespace} command is processed to let
  qdoc know that this namespace is documented in the current module, so that
  when something in another module is marked as related to this namespace, it
  can be documented there with a ProxyNode for this namespace.

  \sa whereDocumented()
 */

/*! \fn void NamespaceNode::setDocumented()
  Sets the flag indicating that the \c {\\namespace} command for this
  namespace was seen.
 */

/*! \fn bool NamespaceNode::wasDocumented() const
  Returns \c true if a \c {\\namespace} command for this namespace was seen.
  Otherwise returns \c false.
 */

/*! \fn void NamespaceNode::setDocNode(NamespaceNode *ns) { m_docNode = ns; }
  Called in QDocDatabase::resolveNamespaces() to set the pointer to the
  NamespaceNode in which this namespace is documented.

  \sa QDocDatabase::resolveNamespaces()
 */

/*! \fn NamespaceNode *NamespaceNode::docNode() const
  Returns a pointer to the NamespaceNode that represents where the namespace
  documentation is actually generated. API elements in many different modules
  can be included in a single namespace. That namespace is only documented in
  one module. The namespace is documented in the module where the \c {\\namespace}
  command for the namespace appears.

  \sa QDocDatabase::resolveNamespaces()
 */

QT_END_NAMESPACE
