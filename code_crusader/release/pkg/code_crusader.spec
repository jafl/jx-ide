Summary: Code Crusader is a UNIX development environment for X.
Name: %app_name
Version: %pkg_version
Release: 1
License: Copyright John Lindal
Group: Development/Tools/IDE
Source: %pkg_name
Requires: editorconfig-libs, libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

%description
Code Crusader is a graphical development environment for UNIX.

%prep
%setup 

%install

%define jcc_doc_dir     /usr/share/doc/code-crusader
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install $RPM_BUILD_ROOT

%files

%docdir %jcc_doc_dir

/usr/bin/code-crusader
/usr/bin/jcc
/usr/bin/xml-auto-close
/usr/bin/html-auto-close
/usr/bin/java-import
%jcc_doc_dir

%gnome_app_path/code_crusader.desktop
%gnome_icon_path/code_crusader.png
