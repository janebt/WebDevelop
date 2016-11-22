include $(TOPDIR)/rules.mk

PKG_NAME:=websocketd
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/websocketd
	SUBMENU:=Apps
	SECTION:=utils
	CATEGORY:=TP-LINK Proprietary Software
	TITLE:=websocketd -- tp websocketd module.
	DEPENDS:= +libjson +libubox +libubus +libuci +libsyslog +public
	MENU:=1
endef

define Package/websocketd/description
	websocketd package....
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	find $(PKG_BUILD_DIR)/ -name ".svn" | xargs rm -rf
endef

define Package/websocketd/install
	#$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/websocketd $(1)/bin/
	#cp -f $(PKG_BUILD_DIR)/files/etc/init.d/websocketd $(1)/etc/init.d
endef

$(eval $(call BuildPackage,websocketd))
