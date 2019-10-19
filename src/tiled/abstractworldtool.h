/*
 * abstractobjecttool.h
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

#pragma once

#include "abstracttool.h"

class QAction;

namespace Tiled {

class MapObject;
class ObjectGroup;
struct World;
class MapObjectItem;

/**
 * A convenient base class for tools that work on object layers. Implements
 * the standard context menu.
 */
class AbstractWorldTool : public AbstractTool
{
    Q_OBJECT
    Q_INTERFACES(Tiled::AbstractTool)

public:
    /**
     * Constructs an abstract object tool with the given \a name and \a icon.
     */
    AbstractWorldTool(const QString &name,
                       const QIcon &icon,
                       const QKeySequence &shortcut,
                       QObject *parent = nullptr);

    void activate(MapScene *scene) override;
    void deactivate(MapScene *scene) override;

    void keyPressed(QKeyEvent *event) override;
    void mouseLeft() override;
    void mouseMoved(const QPointF &pos, Qt::KeyboardModifiers modifiers) override;
    void mousePressed(QGraphicsSceneMouseEvent *event) override;

    void languageChanged() override;

    void populateToolBar(QToolBar*) override;

protected:
    /**
     * Overridden to only enable this tool when the currently has a world loaded
     */
    void updateEnabledState() override;

    void addToWorld();
    void removeFromWorld();

    QPoint snapPoint(QPoint point) const;
    bool currentMapCanBeMoved() const;
    QRect currentMapRect() const;
    const World *currentConstWorld() const;
    QPoint currentTileSize();
    MapScene *mapScene() const { return mMapScene; }

    void showContextMenu(QPoint screenPos);

    MapScene *mMapScene;
    QPoint mMousePos;
};

} // namespace Tiled
