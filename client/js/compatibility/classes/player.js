/// <reference path="../../../../types/shared/index.d.ts" />
/// <reference path="../../../../types/client/index.d.ts" />
// import * as alt from "@altv/client";

requireBinding("shared/factory.js");

const { SharedPlayer } = requireBinding("shared/compatibility/classes/sharedPlayer.js");

const { Entity } = requireBinding("client/compatibility/classes/entity.js");
const { WorldObject } = requireBinding("client/compatibility/classes/worldObject.js");
const { BaseObject } = requireBinding("client/compatibility/classes/baseObject.js");

const { extendAltEntityClass } = requireBinding("shared/compatibility/utils/classes.js");

class Player extends alt.Player {
    constructor() {
        super();

        extendAltEntityClass(this, SharedPlayer, Entity, WorldObject, BaseObject);
    }
}

alt.Factory.setPlayerFactory(Player);
cppBindings.registerCompatibilityExport("Player", Player);