/*=========================================================================

   Program:   ParaQ
   Module:    pqPipelineBuilder.h

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaQ is a free software; you can redistribute it and/or modify it
   under the terms of the ParaQ license version 1.1. 

   See License_v1.1.txt for the full ParaQ license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#ifndef _pqPipelineBuilder_h
#define _pqPipelineBuilder_h

#include <QObject>
#include "pqWidgetsExport.h"

class vtkSMDisplayProxy;
class vtkSMProxy;
class vtkSMRenderModuleProxy;
class vtkSMSourceProxy;

class pqNameCount;
class pqPipelineSource;
class pqRenderModule;
class pqServer;
class pqUndoStack;

/// This is a class that can build pipelines. Use this class to create 
/// sources/filters/readers etc etc. This class will ensure that all
/// ServerManager creation checklist is met. Display proxies are created 
/// for each created source/filter/reader automatically. 
/// This class must be used to set up source/sink connections as well 
/// as breaking them. Since it manages the UndoStack, every operation
/// is recorded as an undoable state. 
/// Another point to note about pqPipelineBuilder is that it never
/// call EndUndoSet() on the pqUndoStack, instead always pauses the
/// recording of the undo set. That way it can collapse mutiple 
/// operations into on undoable set.

class PQWIDGETS_EXPORT pqPipelineBuilder : public QObject
{
  Q_OBJECT

public:
  /// returns the global instance of the pqPipelineBuilder.
  static pqPipelineBuilder* instance();  

public:
  pqPipelineBuilder(QObject* parent=0);
  virtual ~pqPipelineBuilder();

  /// Get/Set the undo stack to use while creating proxies.
  /// I cannot decide if the pipeline builder should directly reference 
  /// a pqUndoStack or emit signals that are hooked on the appropriate
  /// slots on pqUndoStack. For now, let's just keep a reference.
  void setUndoStack(pqUndoStack* s)
    { this->UndoStack = s; }
  pqUndoStack* undoStack()
    { return this->UndoStack; }
  
  /// Creates a source/filter/reader proxy with the given xmlgroup,xmlname.
  /// The proxy is registered with a unique name under the group "sources".
  /// If renModule is non-null, this also creates (and registers) a display 
  /// proxy for the proxy, and adds it to the render module. All the work
  /// done by a call to this method gets captured in a UndoSet, thus it
  /// can be undone is a single step. It is essential that renModule is 
  /// a render module on the server. renModule can be null, in which the 
  /// display wont be created. If xmlgroup is NULL, this method will
  /// try to instantiate a compound proxy with the given name.
  pqPipelineSource* createSource(const char* xmlgroup,
    const char* xmlname, pqServer* server, pqRenderModule* renModule);

  // Create a display proxy for the given proxy(source/filter) and add 
  // it to the given render module. 
  vtkSMDisplayProxy* createDisplayProxy(pqPipelineSource* src,
    pqRenderModule* renModule);

  /// Create new viewing "window" on the server. It uses the 
  /// MultiViewRenderModule on the given server to instantiate a new render 
  /// module.
  pqRenderModule* createWindow(pqServer* server);

  /// Remove a viewing "window".
  void removeWindow(pqRenderModule* rm);

  // Create a connection between a source and a sink. This method ensures
  // that the UndoState is recoreded. Remember this "connection" is not a 
  // server connection, but connection between two pipeline objects.
  void addConnection(pqPipelineSource* source, pqPipelineSource* filter);

  // Removes a connection between a source and a sink. This method ensures that 
  // the UndoState is recorded.
  void removeConnection(vtkSMProxy* source, vtkSMProxy* sink);

  // This method unregisters all proxies on the given server. This for now,
  // is a non-undoable change. 
  void deleteProxies(pqServer* server);

protected:
  /// this method does what it says. Note that it does not worry about undo stack
  /// at all. The caller would have managed it.
  vtkSMDisplayProxy* createDisplayProxyInternal(
    vtkSMSourceProxy* proxy, vtkSMRenderModuleProxy*);

  // Internal create method.
  vtkSMProxy* createPipelineProxy(const char* xmlgroup,
    const char* xmlname, pqServer* server, pqRenderModule* renModule);   

  /// internal implementation to addConnection.
  void addConnection(vtkSMProxy* source, vtkSMProxy* sink);

  pqNameCount* NameGenerator;
  pqUndoStack* UndoStack;

private:
  static pqPipelineBuilder* Instance;
};


#endif

