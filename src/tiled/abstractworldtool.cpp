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

#include "documentmanager.h"
#include "mapdocument.h"
#include "map.h"
#include "mapscene.h"
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
        //showContextMenu(topMostMapObjectAt(event->scenePos()),
        //                event->screenPos());
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

QRect AbstractWorldTool::currentMapRect() const
{
    const World* pWorld = currentConstWorld();
    QRect rect = pWorld->mapRect( mapDocument()->fileName() );
    return QRect(rect.topLeft(), mapDocument()->map()->size());
    //rect.setSize(mapDocument()->map()->size());
    //return rect;
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
void AbstractWorldTool::showContextMenu(MapObject *clickedObject,
                                         QPoint screenPos)
{
//    const QList<MapObject*> &selectedObjects = mapDocument()->selectedObjects();

//    if (clickedObject && !selectedObjects.contains(clickedObject))
//        mapDocument()->setSelectedObjects({ clickedObject });

//    if (selectedObjects.isEmpty())
//        return;

//    QMenu menu;
//    QAction *duplicateAction = menu.addAction(tr("Duplicate %n Object(s)", "", selectedObjects.size()),
//                                              this, &AbstractObjectTool::duplicateObjects);
//    QAction *removeAction = menu.addAction(tr("Remove %n Object(s)", "", selectedObjects.size()),
//                                           this, &AbstractObjectTool::removeObjects);

//    duplicateAction->setIcon(QIcon(QLatin1String(":/images/16/stock-duplicate-16.png")));
//    removeAction->setIcon(QIcon(QLatin1String(":/images/16/edit-delete.png")));

//    bool anyTileObjectSelected = std::any_of(selectedObjects.begin(),
//                                             selectedObjects.end(),
//                                             isTileObject);

//    if (anyTileObjectSelected) {
//        auto resetTileSizeAction = menu.addAction(tr("Reset Tile Size"), this, &AbstractObjectTool::resetTileSize);
//        resetTileSizeAction->setEnabled(std::any_of(selectedObjects.begin(),
//                                                    selectedObjects.end(),
//                                                    isResizedTileObject));

//        auto changeTileAction = menu.addAction(tr("Replace Tile"), this, &AbstractObjectTool::changeTile);
//        changeTileAction->setEnabled(tile() && (!selectedObjects.first()->isTemplateBase() ||
//                                                tile()->tileset()->isExternal()));
//    }

//    // Create action for replacing an object with a template
//    auto replaceTemplateAction = menu.addAction(tr("Replace With Template"), this, &AbstractObjectTool::replaceObjectsWithTemplate);
//    auto selectedTemplate = objectTemplate();

//    if (selectedTemplate) {
//        QString name = QFileInfo(selectedTemplate->fileName()).fileName();
//        replaceTemplateAction->setText(tr("Replace With Template \"%1\"").arg(name));
//    }
//    if (!selectedTemplate || !mapDocument()->templateAllowed(selectedTemplate))
//        replaceTemplateAction->setEnabled(false);

//    if (selectedObjects.size() == 1) {
//        MapObject *currentObject = selectedObjects.first();

//        if (!(currentObject->isTemplateBase() || currentObject->isTemplateInstance())) {
//            const Cell cell = selectedObjects.first()->cell();
//            // Saving objects with embedded tilesets is disabled
//            if (cell.isEmpty() || cell.tileset()->isExternal())
//                menu.addAction(tr("Save As Template"), this, &AbstractObjectTool::saveSelectedObject);
//        }

//        if (currentObject->isTemplateBase()) { // Hide this operations for template base
//            duplicateAction->setVisible(false);
//            removeAction->setVisible(false);
//            replaceTemplateAction->setVisible(false);
//        }
//    }

//    bool anyTemplateInstanceSelected = std::any_of(selectedObjects.begin(),
//                                                   selectedObjects.end(),
//                                                   isTemplateInstance);

//    if (anyTemplateInstanceSelected) {
//        menu.addAction(tr("Detach"), this, &AbstractObjectTool::detachSelectedObjects);

//        auto resetToTemplateAction = menu.addAction(tr("Reset Template Instance(s)"), this, &AbstractObjectTool::resetInstances);
//        resetToTemplateAction->setEnabled(std::any_of(selectedObjects.begin(),
//                                                      selectedObjects.end(),
//                                                      isChangedTemplateInstance));
//    }

//    menu.addSeparator();
//    menu.addAction(tr("Flip Horizontally"), this, &AbstractObjectTool::flipHorizontally, QKeySequence(tr("X")));
//    menu.addAction(tr("Flip Vertically"), this, &AbstractObjectTool::flipVertically, QKeySequence(tr("Y")));

//    ObjectGroup *sameObjectGroup = RaiseLowerHelper::sameObjectGroup(selectedObjects);
//    if (sameObjectGroup && sameObjectGroup->drawOrder() == ObjectGroup::IndexOrder) {
//        menu.addSeparator();
//        menu.addAction(tr("Raise Object"), this, &AbstractObjectTool::raise, QKeySequence(tr("PgUp")));
//        menu.addAction(tr("Lower Object"), this, &AbstractObjectTool::lower, QKeySequence(tr("PgDown")));
//        menu.addAction(tr("Raise Object to Top"), this, &AbstractObjectTool::raiseToTop, QKeySequence(tr("Home")));
//        menu.addAction(tr("Lower Object to Bottom"), this, &AbstractObjectTool::lowerToBottom, QKeySequence(tr("End")));
//    }

//    auto objectGroups = mapDocument()->map()->objectGroups();
//    if (!objectGroups.isEmpty()) {
//        menu.addSeparator();
//        QMenu *moveToLayerMenu = menu.addMenu(tr("Move %n Object(s) to Layer",
//                                                 "", selectedObjects.size()));
//        for (Layer *layer : objectGroups) {
//            ObjectGroup *objectGroup = static_cast<ObjectGroup*>(layer);
//            QAction *action = moveToLayerMenu->addAction(objectGroup->name());
//            action->setData(QVariant::fromValue(objectGroup));
//            action->setEnabled(objectGroup != sameObjectGroup);
//        }
//    }

//    menu.addSeparator();
//    QIcon propIcon(QLatin1String(":images/16/document-properties.png"));
//    QAction *propertiesAction = menu.addAction(propIcon,
//                                               tr("Object &Properties..."));

//    Utils::setThemeIcon(removeAction, "edit-delete");
//    Utils::setThemeIcon(propertiesAction, "document-properties");

//    QAction *action = menu.exec(screenPos);
//    if (!action)
//        return;

//    if (action == propertiesAction) {
//        MapObject *mapObject = selectedObjects.first();
//        mapDocument()->setCurrentObject(mapObject);
//        emit mapDocument()->editCurrentObject();
//        return;
//    }

//    if (ObjectGroup *objectGroup = action->data().value<ObjectGroup*>()) {
//        const auto selectedObjectsCopy = selectedObjects;
//        mapDocument()->moveObjectsToGroup(selectedObjects, objectGroup);
//        mapDocument()->setSelectedObjects(selectedObjectsCopy);
//    }
}
