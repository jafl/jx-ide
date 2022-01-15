Summary: Code Mill is a Code Crusader plug-in for creating C++ derived classes.
Name: %app_name
Version: %pkg_version
Release: 1
License: Copyright John Lindal
Group: Development/Code Generators
Source: %pkg_name
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

%description
Code Mill is a Code Crusader plug-in for creating C++ derived classes.

%prep
%setup 

%install

%define code_mill_doc_dir /usr/share/doc/code-mill
%define gnome_app_path    /usr/share/applications

./install $RPM_BUILD_ROOT

%files

%docdir %code_mill_doc_dir

/usr/bin/code_mill
%code_mill_doc_dir

%gnome_app_path/code_mill.desktop
