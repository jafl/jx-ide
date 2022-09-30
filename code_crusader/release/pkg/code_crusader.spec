Summary: Code Crusader is a UNIX development environment for X.
Name: %app_name
Version: %pkg_version
Release: 1
License: Copyright John Lindal
Group: Development/Tools/IDE
Source: %pkg_name
Requires: editorconfig-libs, libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre, ctags

%description
Code Crusader is a graphical development environment for UNIX.

%prep
%setup 

%install

%define jcc_doc_dir     /usr/local/share/doc/code-crusader
%define gnome_app_path  /usr/local/share/applications
%define gnome_icon_path /usr/local/share/pixmaps

./install $RPM_BUILD_ROOT/usr/local

%files

%docdir %jcc_doc_dir

/usr/local/bin/code-crusader
/usr/local/bin/jcc
/usr/local/bin/xml-auto-close
/usr/local/bin/html-auto-close
/usr/local/bin/java-import
%jcc_doc_dir

%gnome_app_path/code_crusader.desktop
%gnome_icon_path/code_crusader.png
