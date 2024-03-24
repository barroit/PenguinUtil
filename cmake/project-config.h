#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#define PROJECT_VERSION	"${PassKeeper_VERSION_MAJOR}.${PassKeeper_VERSION_MINOR}.${PassKeeper_VERSION_PATCH}"

#define ARCHITECTURE	"$ENV{ARCHITECTURE}"
#define BUILD_COMMIT	"$ENV{BUILD_COMMIT}"

#define AUTHOR		"$ENV{PK_AUTHOR}"
#define CONTACT		"$ENV{PK_AUTHOR_CONTACT}"
#define PROJNAME	"$ENV{PK_PROJNAME}"
#define LICENSE_NOTIES	"$ENV{PK_LICENSE_NOTIES}"

#define PK_CRED_DB	"PK_CRED_DB"
#define PK_CRED_KY	"PK_CRED_KY"
#define PK_CRED_DB_NM	".pk-credfl"
#define PK_CRED_KY_NM	".pk-credky"

#endif /* PROJECT_CONFIG_H */