/* See LICENSE file for copyright and license details. */
#include "selfrestart.c"
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int gappx     = 16;       /* gaps between windows on tiling mode*/
static const unsigned int snap      = 20;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 16;  /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int horizpadbar        = 6;        /* horizontal padding for statusbar */
static const int vertpadbar         = 8;        /* vertical padding for statusbar */
static const char *fonts[]           = {
                                        "monospace:size=12:antialias=true:autohint=true",
                                        "siji:size=12:antialias=true:autohint=true"
};
static const char dmenufont[]       = "monospace:size=13";
static const char col_background[]  = "#222222";
static const char col_mid1[]        = "#4f4f4f";
static const char col_mid2[]        = "#aeaeae";
static const char col_foreground[]  = "#fcfcfc";
static const char col_highlight[]   = "#3498d8";
static const char *colors[][3]      = {
                                       /*               fg         bg         border   */
                                       [SchemeNorm] = { col_foreground, col_background, col_background },
                                       [SchemeSel]  = { col_foreground, col_highlight,  col_highlight  },
};

/* tagging */
#define MAX_TAGNAME_LEN 14
#define TAG_PREPEND "%1i:"
#define MAX_TAGLEN 16
static char tags[][MAX_TAGLEN] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
                             /* xprop(1):
                              *	WM_CLASS(STRING) = instance, class
                              *	WM_NAME(STRING) = title
                              */
                             /* class          instance    title       tags mask     isfloating   monitor */
                             { "Gimp",         NULL,       NULL,       0,            True,        -1 },
                             { "xpad",         NULL,       NULL,       0,            True,        -1 },
                             { "Gcolor2",      NULL,       NULL,       0,            True,        -1 },
};


/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
                                 /* symbol  arrange function */
                                 { "",     tile },    /* first entry is default */
                                 { "",     NULL },    /* no layout function means floating behavior */
                                 { "",      monocle },
                                 { "",     grid },
                                 { "",     spiral },
                                 { "",     dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG)                                                      \
{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_background, "-nf", col_mid2, "-sb", col_highlight, "-sf", col_foreground, NULL };
static const char *dmenuexcmd[]    = { "dmenu_extended_run", NULL };
static const char *termcmd[]       = { "alacritty", NULL };
static const char *webbrowsecmd[]  = { "firefox", NULL };
static const char *editorcmd[]     = { "emacs", NULL };
static const char *filecmd[]       = { "thunar", NULL };
static const char *evolutioncmd[]  = { "evolution", NULL };
static const char *photoscmd[]     = { "shotwell", NULL };
static const char *passwordscmd[]  = { "bitwarden", NULL };
static const char *lockcmd[]       = { "physlock", NULL };
static const char *networkcmd[]    = { "nm-connection-editor", NULL };
static const char *screenshotcmd[] = { "xfce4-screenshooter", NULL };
static const char *pavucontrol[]   = { "pavucontrol", NULL };
static const char *mutecmd[]       = { "amixer", "-q", "set", "Master", "toggle", NULL };
static const char *volupcmd[]      = { "amixer", "-q", "set", "Master", "5%+", "unmute", NULL };
static const char *voldowncmd[]    = { "amixer", "-q", "set", "Master", "5%-", "unmute", NULL };
static const char *brupcmd[]       = { "sudo", "xbacklight", "-inc", "10", NULL };
static const char *brdowncmd[]     = { "sudo", "xbacklight", "-dec", "10", NULL };




static Key keys[] = {
                     /* modifier                     key        function        argument */
                     { MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
                     { ControlMask,                  XK_space,  spawn,          {.v = dmenuexcmd } },
                     { MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
                     { MODKEY|ShiftMask,             XK_b,      spawn,          {.v = passwordscmd } },
                     { MODKEY|ShiftMask,             XK_e,      spawn,          {.v = editorcmd } },
                     { MODKEY|ShiftMask,             XK_f,      spawn,          {.v = filecmd } },
                     { MODKEY|ShiftMask,             XK_l,      spawn,          {.v = lockcmd } },
                     { MODKEY|ShiftMask,             XK_m,      spawn,          {.v = evolutioncmd } },
                     { MODKEY|ShiftMask,             XK_n,      spawn,          {.v = networkcmd } },
                     { MODKEY|ShiftMask,             XK_p,      spawn,          {.v = photoscmd } },
                     { MODKEY|ShiftMask,             XK_s,      spawn,          {.v = screenshotcmd } },
                     { MODKEY|ShiftMask,             XK_v,      spawn,          {.v = pavucontrol } },
                     { MODKEY|ShiftMask,             XK_w,      spawn,          {.v = webbrowsecmd } },
                     { MODKEY,                       XK_b,      togglebar,      {0} },
                     { MODKEY|ShiftMask,             XK_j,      rotatestack,    {.i = +1 } },
                     { MODKEY|ShiftMask,             XK_k,      rotatestack,    {.i = -1 } },
                     { MODKEY,                       XK_Tab,    focusstack,     {.i = +1 } },
                     { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
                     { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
                     { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
                     { MODKEY,                       XK_o,      incnmaster,     {.i = -1 } },
                     { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
                     { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
                     { MODKEY,                       XK_Return, zoom,           {0} },
                     { MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
                     { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
                     { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
                     { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
                     { MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
                     { MODKEY,                       XK_s,      setlayout,      {.v = &layouts[4]} },
                     { MODKEY,                       XK_d,      setlayout,      {.v = &layouts[5]} },
                     { MODKEY,                       XK_space,  setlayout,      {0} },
                     { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
                     { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
                     { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
                     { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
                     { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
                     { MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
                     { MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
                     { MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
                     { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
                     { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
                     { MODKEY,                       XK_n,      nametag,        {0} },
                     TAGKEYS(                        XK_1,                      0)
                     TAGKEYS(                        XK_2,                      1)
                     TAGKEYS(                        XK_3,                      2)
                     TAGKEYS(                        XK_4,                      3)
                     TAGKEYS(                        XK_5,                      4)
                     TAGKEYS(                        XK_6,                      5)
                     TAGKEYS(                        XK_7,                      6)
                     TAGKEYS(                        XK_8,                      7)
                     TAGKEYS(                        XK_9,                      8)
                     { MODKEY|ShiftMask,             XK_r,      self_restart,   {0} },
                     { MODKEY|ShiftMask,             XK_q,      quit,           {0} },
                     { 0, XF86XK_AudioMute, spawn, {.v = mutecmd } },
                     { 0, XF86XK_AudioLowerVolume, spawn, {.v = voldowncmd } },
                     { 0, XF86XK_AudioRaiseVolume, spawn, {.v = volupcmd } },
                     { 0, XF86XK_MonBrightnessUp, spawn, {.v = brupcmd} },
                     { 0, XF86XK_MonBrightnessDown, spawn, {.v = brdowncmd} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
                           /* click                event mask      button          function        argument */
                           { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
                           { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
                           { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
                           { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
                           { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
                           { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
                           { ClkTagBar,            0,              Button1,        view,           {0} },
                           { ClkTagBar,            0,              Button3,        toggleview,     {0} },
                           { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
                           { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
