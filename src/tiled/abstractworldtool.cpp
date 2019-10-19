/*
 * abstractobjecttool.cpp
 * Copyright 2011, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "abstractworldtool.h"

#include "preferences.h"
#include "documentmanager.h"
#include "mapdocument.h"
#include "map.h"
#include "mapscene.h"
#include "mapeditor.h"
#include "maprenderer.h"
#include "tile.h"
#include "utils.h"
#include "worldmanager.h"

#include <QFileDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>
#include <QUndoStack>

#include <QtMath>

#include "qtcompat_p.h"

using namespace Tiled;



AbstractWorldTool::AbstractWorldTool(const QString &name,
                                     const QIcon &icon,
                                     const QKeySequence &shortcut,
                                     QObject *parent)
    : AbstractTool(name, icon, shortcut, parent)
    , mMapScene(nullptr)
{
    AbstractWorldTool::languageChanged();

    WorldManager &worldManager = WorldManager::instance();
    connect(&worldManager, &WorldManager::worldsChanged, this, &AbstractWorldTool::updateEnabledState);
}

void AbstractWorldTool::activate(MapScene *scene)
{
    mMapScene = scene;
}

void AbstractWorldTool::deactivate(MapScene *)
{
    mMapScene = nullptr;
}

void AbstractWorldTool::keyPressed(QKeyEvent *event)
{
    event->ignore();
}

void AbstractWorldTool::mouseLeft()
{
    setStatusInfo(QString());
}

void AbstractWorldTool::mouseMoved(const QPointF &pos,
                                    Qt::KeyboardModifiers)
{
    // Take into account the offset of the current layer
    QPointF offsetPos = pos;
    if (Layer *layer = currentLayer())
        offsetPos -= layer->totalOffset();

    const QPoint pixelPos = offsetPos.toPoint();

    const QPointF tilePosF = mapDocument()->renderer()->screenToTileCoords(offsetPos);
    const int x = qFloor(tilePosF.x());
    const int y = qFloor(tilePosF.y());
    setStatusInfo(QString(QLatin1String("%1, %2 (%3, %4)")).arg(x).arg(y).arg(pixelPos.x()).arg(pixelPos.y()));
}

void AbstractWorldTool::mousePressed(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        showContextMenu(event->screenPos());
    }
}

void AbstractWorldTool::languageChanged()
{

}

void AbstractWorldTool::populateToolBar(QToolBar *toolBar)
{

}

void AbstractWorldTool::updateEnabledState()
{
    setEnabled(currentConstWorld() != nullptr);
}

bool AbstractWorldTool::currentMapCanBeMoved() const
{
    if( ! mapDocument() )
    {
        return false;
    }
    return WorldManager::instance().mapCanBeModified( mapDocument()->fileName() );
}

QRect AbstractWorldTool::currentMapRect() const
{
    const World* pWorld = currentConstWorld();
    QRect rect = pWorld->mapRect( mapDocument()->fileName() );
    QSize size = mapDocument()->map()->size();
    size.setWidth(size.width() * mapDocument()->map()->tileWidth());
    size.setHeight(size.height() * mapDocument()->map()->tileHeight());
    return QRect(rect.topLeft(), size);
}

const World *AbstractWorldTool::currentConstWorld() const
{
    if( !mapDocument() )
    {
        return nullptr;
    }
    return WorldManager::instance().worldForMap( mapDocument()->fileName() );
}

QPoint AbstractWorldTool::currentTileSize()
{
    if( !mapDocument() )
    {
        return QPoint( 16, 16 );
    }
    QSize size = mapDocument()->map()->tileSize();
    return QPoint( size.width(), size.height() );
}



/**
 * Shows the context menu for map objects. The menu allows you to duplicate and
 * remove the map objects, or to edit their properties.
 */
void AbstractWorldTool::showContextMenu(QPoint screenPos)
{
    mMousePos = mapDocument()->renderer()->screenToPixelCoords(screenPos).toPoint();

    QMenu menu;
    QAction *removeFromWorldAction = menu.addAction(tr("Remove current map from world"),
                                              this, &AbstractWorldTool::removeFromWorld, QKeySequence(tr("R") ) );
    QAction *addToWorldAction = menu.addAction(tr("Add a map to the world"),
                                              this, &AbstractWorldTool::addToWorld, QKeySequence(tr("A") ) );

    removeFromWorldAction->setIcon(QIcon(QLatin1String(":/images/16/remove.png")));
    addToWorldAction->setIcon(QIcon(QLatin1String(":/images/16/add.png")));

    QAction *action = menu.exec(screenPos);
    if (!action)
        return;

}

void AbstractWorldTool::addToWorld()
{
    const World* constWorld = currentConstWorld();
    if( ! constWorld )
    {
        return;
    }

    QDir dir = QFileInfo(constWorld->fileName).dir();

    QString lastPath = QDir::cleanPath(dir.absolutePath());
    QString filter = tr("All Files (*);;");
    QString mapFilesFilter = tr("Map Files (*.tmx)");
    filter.append(mapFilesFilter);

    MapEditor* mapEditor = static_cast<MapEditor*>(DocumentManager::instance()->editor(Document::DocumentType::MapDocumentType));
    QString mapFile = QFileDialog::getOpenFileName(mapEditor->editorWidget(), tr("Load Map"), lastPath,
                                                     filter, &mapFilesFilter);
    if (mapFile.isEmpty())
        return;

    QSize size(0,0);
    QRect rect = QRect(snapPoint(mMousePos), size);

    WorldManager::instance().addMap( constWorld, mapFile, rect );
}

QPoint AbstractWorldTool::snapPoint(QPoint point) const
{
    point.setX( point.x() - (point.x())  % mapDocument()->map()->tileWidth());
    point.setY( point.y() - (point.y())  % mapDocument()->map()->tileHeight());
    return point;
}

void AbstractWorldTool::removeFromWorld()
{
    WorldManager::instance().removeMap( mapDocument()->fileName() );
}
