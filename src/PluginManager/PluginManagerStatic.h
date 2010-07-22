#ifndef Map2X_PluginManager_PluginManagerStatic_h
#define Map2X_PluginManager_PluginManagerStatic_h
/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Map2X.

    Map2X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Map2X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Map2X::PluginManager::PluginManagerStatic
 */

#include <vector>
#include <string>

#include "PluginMetadata.h"

namespace Map2X { namespace PluginManager {

/**
 * @brief Base class of PluginManager
 *
 * Defines static members which are used by all PluginManager template classes.
 * See also @ref PluginManagement.
 */
class PluginManagerStatic {
    public:
        /**
         * @brief Load state
         *
         * Describes state of the plugin. Values below zero are used when
         * loading plugin, values above zero when unloading plugin. Static
         * plugins are loaded at first, they have always state
         * PluginMetadataStatic::IsStatic. Dynamic plugins have at first state
         * NotLoaded, after first attempt to load the state is changed.
         */
        enum LoadState {
            /** %Plugin cannot be found */
            NotFound = -7,

            /**
             * The plugin is build with different version of PluginManager and
             * cannot be loaded. This means the PluginMetadata are inaccessible.
             */
            WrongPluginVersion = -6,

            /**
             * The plugin uses different interface than the interface
             * used by PluginManager and cannot be loaded.
             */
            WrongInterfaceVersion = -5,

            /**
             * The plugin is conflicting with another plugin that is already
             * loaded. Unload conflicting plugin and try it again.
             */
            Conflicts = -4,

            /**
             * The plugin depends on another plugin, which cannot be loaded
             * (e.g. not found, conflict, wrong version).
             */
            UnresolvedDependency = -3,

            /** %Plugin failed to load */
            LoadFailed = -2,

            /** %Plugin is successfully loaded */
            LoadOk = -1,

            /** %Plugin is not yet loaded and its state is unknown */
            Unknown = 0,

            /**
             * %Plugin is not loaded. %Plugin can be unloaded only
             * if is dynamic and is not required by any other plugin.
             */
            NotLoaded = 1,

            /** %Plugin failed to unload */
            UnloadFailed = 2,

            /**
             * %Plugin cannot be unloaded because another plugin is depending on
             * it. Unload that plugin first and try again.
             */
            IsRequired = 3,

            /** %Plugin is static (and cannot be unloaded) */
            IsStatic = 4
        };

        /** @brief %Plugin version */
        static const int version = PLUGIN_VERSION;

        /**
         * @brief Register static plugin
         * @param name              Static plugin name (defined with
         *      PLUGIN_REGISTER_STATIC())
         * @param _version          %Plugin version (must be the same as
         *      PluginManagerStatic::version)
         * @param metadataCreator   Pointer to metadata creator function
         * @param instancer         Pointer to plugin class instancer function
         *
         * Used internally by PLUGIN_IMPORT_STATIC() macro. There is absolutely
         * no need to use this directly.
         */
        static void importStaticPlugin(const std::string& name, int _version, void (*metadataCreator)(PluginMetadata*), void* (*instancer)());

    protected:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        struct StaticPlugin {
            std::string name;
            void (*metadataCreator)(PluginMetadata*);
            void* (*instancer)();
        };

        struct Plugin {
            LoadState loadState;
            PluginMetadata metadata;
            void* (*instancer)();
            void* handle;
            Plugin(): loadState(Unknown), handle(0) {}
        };

        static std::vector<StaticPlugin> staticPlugins;

        inline PluginManagerStatic() {}
        #endif
};

/**
 * @brief Import static plugin
 * @param name      Static plugin name (defined with PLUGIN_REGISTER_STATIC())
 * @hideinitializer
 *
 * Imports statically linked plugin and makes it available in
 * PluginManager. The plugin must be registered with PLUGIN_STATIC_REGISTER()
 * macro, otherwise it will not be loaded.
 */
#define PLUGIN_IMPORT_STATIC(name) \
    Map2X::PluginManager::PluginManagerStatic::importStaticPlugin(#name, name##Version(), name##MetadataCreator, name##Instancer);

}}

#endif