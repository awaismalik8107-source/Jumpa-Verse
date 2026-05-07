#!/usr/bin/env python3
import html
import math
import os
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs_report"
FIG = OUT / "figures"
HTML_OUT = OUT / "JumpaVerse_SDL2_CPP_Game_Documentation.html"

W, H = 1500, 900
COLORS = {
    "bg": (248, 250, 252),
    "ink": (20, 24, 31),
    "muted": (91, 103, 116),
    "orange": (255, 127, 0),
    "blue": (63, 125, 188),
    "green": (67, 160, 110),
    "red": (190, 65, 60),
    "violet": (122, 91, 180),
    "slate": (43, 51, 63),
    "line": (198, 207, 217),
}


def font(size=34, bold=False):
    candidates = [
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf" if bold else "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Bold.ttf" if bold else "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
    ]
    for path in candidates:
        if Path(path).exists():
            return ImageFont.truetype(path, size)
    return ImageFont.load_default()


F_TITLE = font(48, True)
F_H = font(34, True)
F = font(27)
F_SMALL = font(22)
F_MONO = font(21)


def wrap(draw, text, max_width, fnt):
    words = text.split()
    lines, line = [], ""
    for word in words:
        test = (line + " " + word).strip()
        if draw.textbbox((0, 0), test, font=fnt)[2] <= max_width:
            line = test
        else:
            if line:
                lines.append(line)
            line = word
    if line:
        lines.append(line)
    return lines


def text(draw, xy, value, fnt=F, fill=None, max_width=None, spacing=8):
    fill = fill or COLORS["ink"]
    x, y = xy
    lines = [value] if max_width is None else wrap(draw, value, max_width, fnt)
    for line in lines:
        draw.text((x, y), line, font=fnt, fill=fill)
        y += fnt.size + spacing
    return y


def rounded(draw, box, fill, outline=None, width=3, radius=24):
    draw.rounded_rectangle(box, radius=radius, fill=fill, outline=outline, width=width)


def arrow(draw, p1, p2, fill=None, width=5):
    fill = fill or COLORS["slate"]
    draw.line([p1, p2], fill=fill, width=width)
    ang = math.atan2(p2[1] - p1[1], p2[0] - p1[0])
    size = 18
    pts = [
        p2,
        (p2[0] - size * math.cos(ang - math.pi / 6), p2[1] - size * math.sin(ang - math.pi / 6)),
        (p2[0] - size * math.cos(ang + math.pi / 6), p2[1] - size * math.sin(ang + math.pi / 6)),
    ]
    draw.polygon(pts, fill=fill)


def save(name, title, kind, items=None, accent=None):
    accent = accent or COLORS["orange"]
    variant = sum(ord(c) for c in name) % 6
    img = Image.new("RGB", (W, H), COLORS["bg"])
    d = ImageDraw.Draw(img)
    if variant % 3 == 0:
        d.rectangle((0, 0, W, 84), fill=COLORS["slate"])
        text(d, (42, 18), title, F_TITLE, (255, 255, 255))
        d.rectangle((0, 84, W, 96), fill=accent)
    elif variant % 3 == 1:
        d.rectangle((0, 0, 190, H), fill=accent)
        d.rectangle((190, 0, W, 72), fill=COLORS["slate"])
        text(d, (225, 15), title, F_TITLE, (255, 255, 255))
        for y in range(125, 825, 95):
            d.ellipse((60, y, 130, y + 70), outline=(255, 255, 255), width=4)
    else:
        d.rectangle((0, 0, W, H), fill=(244, 247, 251))
        d.polygon([(0, 0), (W, 0), (W, 145), (0, 85)], fill=COLORS["slate"])
        d.polygon([(0, 85), (W, 145), (W, 174), (0, 114)], fill=accent)
        text(d, (42, 24), title, F_TITLE, (255, 255, 255))
    items = items or []

    if kind == "architecture":
        centers = [(280, 250), (720, 250), (1160, 250), (500, 560), (940, 560)]
        labels = ["main.cpp\nLoop + State", "SDL2\nWindow Renderer Events", "Assets\nTextures Fonts CSV", "Gameplay\nPlayer Ground Trap", "Screens\nMenu Login GameOver"]
        for (cx, cy), lab in zip(centers, labels):
            rounded(d, (cx-170, cy-70, cx+170, cy+70), (255, 255, 255), accent, 4)
            text(d, (cx-135, cy-42), lab, F, max_width=270)
        for a, b in [(0,1),(1,2),(0,3),(3,4),(4,0),(1,3)]:
            arrow(d, centers[a], centers[b], COLORS["blue"])

    elif kind == "flow":
        palette = [accent, COLORS["blue"], COLORS["green"], COLORS["red"], COLORS["violet"], COLORS["slate"]]
        if variant == 0:
            y = 160
            for i, lab in enumerate(items):
                rounded(d, (530, y, 970, y+72), (255,255,255), palette[i % len(palette)], 4)
                text(d, (560, y+18), lab, F, max_width=380)
                if i < len(items)-1:
                    arrow(d, (750, y+72), (750, y+122), COLORS["slate"])
                y += 122
        elif variant == 1:
            x, y = 255, 230
            for i, lab in enumerate(items):
                rounded(d, (x, y, x+280, y+95), (255,255,255), palette[i % len(palette)], 4, 18)
                text(d, (x+22, y+25), lab, F_SMALL, max_width=230)
                nx = x + 360
                ny = y if i % 2 == 0 else y + 160
                if i < len(items)-1:
                    arrow(d, (x+280, y+48), (nx, ny+48), COLORS["slate"])
                x, y = nx, ny
                if x > 1120:
                    x, y = 255, 590
        elif variant == 2:
            center = (760, 470)
            d.ellipse((650, 360, 870, 580), fill=(255,255,255), outline=accent, width=5)
            text(d, (690, 430), "System\nState", F_H, max_width=150)
            r = 310
            for i, lab in enumerate(items):
                ang = -math.pi / 2 + i * 2 * math.pi / max(1, len(items))
                cx = int(center[0] + r * math.cos(ang))
                cy = int(center[1] + r * math.sin(ang))
                rounded(d, (cx-135, cy-50, cx+135, cy+50), (255,255,255), palette[i % len(palette)], 4, 22)
                text(d, (cx-105, cy-24), lab, F_SMALL, max_width=210)
                arrow(d, (cx, cy), center, COLORS["line"], 3)
        elif variant == 3:
            lanes = [(210, "Input"), (470, "Logic"), (730, "Output")]
            for x, lane in lanes:
                d.rectangle((x, 150, x+250, 790), fill=(255,255,255), outline=COLORS["line"], width=3)
                d.rectangle((x, 150, x+250, 205), fill=palette[(x//10) % len(palette)])
                text(d, (x+70, 162), lane, F, (255,255,255))
            for i, lab in enumerate(items):
                lane_x = lanes[i % 3][0] + 25
                y = 245 + (i // 3) * 130
                rounded(d, (lane_x, y, lane_x+200, y+78), (250,250,250), palette[i % len(palette)], 3, 14)
                text(d, (lane_x+16, y+18), lab, F_SMALL, max_width=168)
                if i < len(items)-1:
                    n_lane_x = lanes[(i+1) % 3][0] + 25
                    n_y = 245 + ((i+1) // 3) * 130
                    arrow(d, (lane_x+200, y+39), (n_lane_x, n_y+39), COLORS["slate"], 3)
        elif variant == 4:
            y = 170
            for i, lab in enumerate(items):
                if i % 2 == 1:
                    box = (560, y, 940, y+92)
                    d.polygon([(750, y), (940, y+46), (750, y+92), (560, y+46)], fill=(255,255,255), outline=palette[i % len(palette)])
                    text(d, (655, y+30), lab, F_SMALL, max_width=190)
                else:
                    box = (540, y, 960, y+82)
                    rounded(d, box, (255,255,255), palette[i % len(palette)], 4, 18)
                    text(d, (570, y+22), lab, F_SMALL, max_width=360)
                if i < len(items)-1:
                    arrow(d, (750, y+92), (750, y+132), COLORS["slate"], 4)
                y += 132
        else:
            x = 170
            y = 380
            for i, lab in enumerate(items):
                d.ellipse((x, y-70, x+140, y+70), fill=(255,255,255), outline=palette[i % len(palette)], width=5)
                text(d, (x+26, y-25), lab, F_SMALL, max_width=95)
                if i < len(items)-1:
                    arrow(d, (x+140, y), (x+220, y), COLORS["slate"])
                x += 220

    elif kind == "uml":
        if variant % 2 == 0:
            boxes = [
                (80, 170, 430, 470, "hitBox\n+ x,y,w,h\n+ hitBox()\n+ hitBox(x,y,w,h)"),
                (560, 150, 930, 520, "ground_Class\n+ first_ground\n+ ground: vector<SDL_Rect>\n+ rebuild()\n+ render(renderer)\n+ seterForElements()"),
                (1040, 170, 1410, 470, "trapSpike\n+ setterNew(x)\n+ updatePosition(x,y)\n+ render(renderer, offset)"),
                (320, 610, 690, 820, "player\n+ character: SDL_Rect\n+ jumpVelocity\n+ gravity\n+ Render(renderer)\n+ syncHitBox()"),
                (830, 610, 1200, 820, "GameOver\n- active\n- deathCause\n- savedFinalScore\n+ trigger()\n+ reset()"),
            ]
        else:
            boxes = [
                (120, 170, 500, 390, "main.cpp\n+ resetGameplay()\n+ while(!quit)\n+ render cycle"),
                (620, 135, 1010, 385, "functions.h\n+ init()\n+ physicsDevelop()\n+ menuScreen()\n+ groundRandomgenerator()"),
                (1080, 190, 1430, 430, "SDL2 APIs\n+ renderer\n+ events\n+ textures\n+ fonts"),
                (230, 575, 600, 815, "Screens\n+ LoginScreen\n+ RegisterScreen\n+ ScoreMenu\n+ GameOver"),
                (790, 575, 1220, 815, "Gameplay Objects\n+ player\n+ ground_Class\n+ trapSpike\n+ Score"),
            ]
        for box in boxes:
            rounded(d, box[:4], (255,255,255), COLORS["slate"], 4, 16)
            lines = box[4].split("\n")
            text(d, (box[0]+22, box[1]+18), lines[0], F_H, accent)
            yy = box[1]+70
            d.line((box[0], yy, box[2], yy), fill=COLORS["line"], width=3)
            text(d, (box[0]+22, yy+15), "\n".join(lines[1:]), F_SMALL, max_width=box[2]-box[0]-44)
        for p1, p2, col in [
            ((255, 470), (470, 610), COLORS["violet"]),
            ((255, 470), (560, 310), COLORS["violet"]),
            ((430, 320), (560, 320), COLORS["green"]),
            ((930, 320), (1040, 320), COLORS["green"]),
            ((815, 385), (980, 575), COLORS["blue"]),
        ]:
            arrow(d, p1, p2, col)

    elif kind == "pipeline":
        labs = items or ["Input", "Update", "Camera", "Render", "Present"]
        palette = [accent, COLORS["blue"], COLORS["green"], COLORS["red"], COLORS["violet"]]
        if variant in (0, 3):
            xs = [100, 365, 630, 895, 1160]
            for i, (x, lab) in enumerate(zip(xs, labs)):
                rounded(d, (x, 290, x+210, 460), (255,255,255), palette[i % len(palette)], 5, 28)
                text(d, (x+28, 340), lab, F_H, max_width=155)
            for i in range(min(len(xs), len(labs))-1):
                arrow(d, (xs[i]+210, 375), (xs[i+1], 375), COLORS["slate"])
            d.rectangle((110, 610, 1390, 720), outline=COLORS["blue"], width=5)
            text(d, (140, 642), "CPU builds state; SDL renderer receives draw commands and presents the frame.", F, max_width=1200)
        elif variant in (1, 4):
            x = 220
            y = 175
            for i, lab in enumerate(labs):
                rounded(d, (x, y, x+1040, y+76), (255,255,255), palette[i % len(palette)], 4, 12)
                text(d, (x+30, y+20), f"{i+1}. {lab}", F, max_width=900)
                if i < len(labs)-1:
                    arrow(d, (x+520, y+76), (x+520, y+116), COLORS["slate"])
                y += 116
        else:
            d.ellipse((570, 260, 930, 620), outline=accent, width=9, fill=(255,255,255))
            text(d, (655, 400), "Frame\nPipeline", F_H, max_width=190)
            for i, lab in enumerate(labs):
                ang = i * 2 * math.pi / max(1, len(labs)) - math.pi / 2
                cx = int(750 + 460 * math.cos(ang))
                cy = int(440 + 280 * math.sin(ang))
                rounded(d, (cx-120, cy-45, cx+120, cy+45), (255,255,255), palette[i % len(palette)], 4, 18)
                text(d, (cx-92, cy-22), lab, F_SMALL, max_width=180)
                arrow(d, (cx, cy), (750, 440), COLORS["line"], 3)

    elif kind == "memory":
        labs = items or ["vector slot", "ground_Class", "SDL_Rect tiles", "shared texture pointer"]
        if variant % 2 == 0:
            x = 90
            for i, lab in enumerate(labs):
                rounded(d, (x, 260, x+280, 420), (255,255,255), [accent, COLORS["blue"], COLORS["green"], COLORS["red"]][i % 4], 5, 20)
                text(d, (x+28, 310), lab, F, max_width=220)
                if i < len(labs)-1:
                    arrow(d, (x+280, 340), (x+340, 340), COLORS["slate"])
                x += 350
        else:
            for i, lab in enumerate(labs):
                x = 250 + (i % 2) * 560
                y = 210 + (i // 2) * 230
                d.rectangle((x, y, x+390, y+135), fill=(255,255,255), outline=[accent, COLORS["blue"], COLORS["green"], COLORS["red"]][i % 4], width=5)
                d.rectangle((x, y, x+390, y+34), fill=[accent, COLORS["blue"], COLORS["green"], COLORS["red"]][i % 4])
                text(d, (x+24, y+58), lab, F, max_width=330)
            arrow(d, (640, 278), (810, 278), COLORS["slate"])
            arrow(d, (445, 345), (445, 440), COLORS["slate"])
        text(d, (120, 560), "The game stores geometry by value in vectors, while SDL_Texture pointers are shared renderer resources and are not owned by each ground object.", F, max_width=1260)

    elif kind == "chart":
        vals = [18, 42, 68, 36, 75, 55]
        names = items or ["gap", "height", "width", "trap", "attempt", "score"]
        if variant % 2 == 0:
            d.line((170, 720, 1320, 720), fill=COLORS["slate"], width=5)
            d.line((170, 720, 170, 180), fill=COLORS["slate"], width=5)
            for i, v in enumerate(vals):
                x = 230 + i * 175
                h = int(v * 6)
                d.rectangle((x, 720-h, x+90, 720), fill=[accent, COLORS["blue"], COLORS["green"], COLORS["red"], COLORS["violet"], COLORS["orange"]][i])
                text(d, (x-12, 742), names[i], F_SMALL, max_width=135)
                text(d, (x+8, 690-h), f"{v}%", F_SMALL, COLORS["ink"])
        else:
            center = (750, 460)
            start = -math.pi / 2
            total = sum(vals)
            colors = [accent, COLORS["blue"], COLORS["green"], COLORS["red"], COLORS["violet"], COLORS["orange"]]
            for i, v in enumerate(vals):
                end = start + 2 * math.pi * v / total
                d.pieslice((470, 180, 1030, 740), int(math.degrees(start)), int(math.degrees(end)), fill=colors[i])
                mid = (start + end) / 2
                lx = int(center[0] + 390 * math.cos(mid))
                ly = int(center[1] + 300 * math.sin(mid))
                text(d, (lx-55, ly-18), names[i], F_SMALL, max_width=120)
                start = end
            d.ellipse((630, 340, 870, 580), fill=COLORS["bg"])
            text(d, (683, 420), "Probability", F, max_width=160)

    elif kind == "geometry":
        d.rectangle((100, 650, 1380, 740), fill=(116, 86, 58), outline=COLORS["slate"], width=4)
        d.rectangle((320, 540, 410, 650), fill=(230, 50, 50), outline=COLORS["slate"], width=4)
        d.polygon([(780,650),(880,650),(830,540)], fill=(92,52,40), outline=COLORS["slate"])
        d.rectangle((318, 538, 412, 652), outline=COLORS["blue"], width=6)
        d.rectangle((780, 540, 880, 650), outline=COLORS["red"], width=6)
        arrow(d, (410, 590), (780, 590), COLORS["green"])
        text(d, (500, 510), "Collision uses SDL_Rect intersection against visible trap bounds.", F, max_width=540)

    elif kind == "ui":
        d.rectangle((0, 96, W, H), fill=(30, 30, 30))
        text(d, (520, 135), "Jumpa Verse", F_TITLE, COLORS["orange"])
        for i, lab in enumerate(["Play", "Scores", "Exit"]):
            fill = COLORS["orange"] if i == 0 else (192, 192, 192)
            d.rectangle((500, 310+i*140, 1000, 420+i*140), fill=fill)
            text(d, (700, 345+i*140), lab, F_H, (30,30,30))
        d.line((0, 760, W, 760), fill=(255,255,255), width=4)
        d.rectangle((170, 660, 250, 760), fill=(255,127,0), outline=(40,22,18), width=4)
        d.polygon([(1160,760),(1240,760),(1200,680)], fill=(92,52,40), outline=COLORS["orange"])

    else:
        for i, lab in enumerate(items or ["Theory", "Concept", "Implementation", "Code", "Issue", "Solution"]):
            x = 100 + (i % 3) * 455
            y = 180 + (i // 3) * 270
            rounded(d, (x, y, x+350, y+170), (255,255,255), [accent, COLORS["blue"], COLORS["green"], COLORS["red"], COLORS["violet"], COLORS["slate"]][i % 6], 5)
            text(d, (x+28, y+45), lab, F_H, max_width=290)

    path = FIG / name
    img.save(path, quality=95)
    return path


def read_file(name, limit=90):
    p = ROOT / name
    if not p.exists():
        return ""
    lines = p.read_text(errors="replace").splitlines()
    if len(lines) > limit:
        lines = lines[:limit] + ["// ... excerpt trimmed for the report ..."]
    return "\n".join(lines)


def code_block(code):
    return f"<pre><code>{html.escape(code)}</code></pre>"


def para(text_value):
    return f"<p>{html.escape(text_value)}</p>"


def fig_html(path, caption, explanation):
    rel = path.relative_to(OUT).as_posix()
    return f"""
<figure>
  <img src="{rel}" alt="{html.escape(caption)}" width="490" height="294" />
  <figcaption>{html.escape(caption)}</figcaption>
  <p class="figure-note">{html.escape(explanation)}</p>
</figure>
"""


def sdl_function_table():
    rows = [
        ("SDL_Init(Uint32 flags)", "int", "Initializes SDL subsystems; called in init() before creating the window."),
        ("SDL_CreateWindow(title,x,y,w,h,flags)", "SDL_Window*", "Allocates the native game window and applies fullscreen/resizable flags."),
        ("SDL_CreateRenderer(window,index,flags)", "SDL_Renderer*", "Creates the accelerated rendering context used by all draw calls."),
        ("SDL_PollEvent(SDL_Event*)", "int", "Pulls queued keyboard, mouse, text, quit, and window events inside each loop."),
        ("SDL_GetKeyboardState(int*)", "const Uint8*", "Returns the current keyboard state array used by physicsDevelop()."),
        ("SDL_GetMouseState(int*,int*)", "Uint32", "Reads mouse position/buttons for menu hover and player jump shortcuts."),
        ("SDL_SetRenderDrawColor(renderer,r,g,b,a)", "int", "Selects the drawing color for clear, rectangles, borders, and UI."),
        ("SDL_RenderClear(renderer)", "int", "Clears the current back buffer before drawing the next frame."),
        ("SDL_RenderFillRect(renderer, rect)", "int", "Draws filled UI buttons, player body, and panels."),
        ("SDL_RenderDrawRect(renderer, rect)", "int", "Draws borders for score boxes, controls, and hitbox-style UI."),
        ("SDL_RenderCopy(renderer, texture, src, dst)", "int", "Copies texture pixels into a target rectangle for ground and text."),
        ("SDL_RenderPresent(renderer)", "void", "Swaps the rendered back buffer to the visible screen."),
        ("IMG_Init(flags)", "int", "Initializes SDL_image JPEG support for ground texture loading."),
        ("IMG_LoadTexture(renderer,path)", "SDL_Texture*", "Loads image files directly into GPU texture resources."),
        ("TTF_OpenFont(path,size)", "TTF_Font*", "Loads typefaces used by menus, score display, and screens."),
        ("TTF_RenderText_Blended(font,text,color)", "SDL_Surface*", "Rasterizes text into a temporary CPU surface."),
        ("SDL_CreateTextureFromSurface(renderer,surface)", "SDL_Texture*", "Uploads the text surface into a GPU texture."),
        ("filledTrigonRGBA(renderer,...)", "int", "SDL2_gfx filled triangle used for solid spike traps."),
        ("trigonRGBA(renderer,...)", "int", "SDL2_gfx outline triangle used for trap edges."),
        ("SDL_SetWindowFullscreen(window, flags)", "int", "Toggles desktop fullscreen and windowed mode in custom controls."),
    ]
    body = "".join(f"<tr><td><code>{html.escape(a)}</code></td><td>{b}</td><td>{html.escape(c)}</td></tr>" for a,b,c in rows)
    return f"<table><caption>SDL2 and extension functions used by Jumpa Verse</caption><thead><tr><th>Syntax</th><th>Return</th><th>Project role</th></tr></thead><tbody>{body}</tbody></table>"


def chapter(num, title, theme, files, figures, special=""):
    file_summary = ", ".join(files) if files else "cross-module behavior"
    intro = f"""
<h1>Chapter {num} - {html.escape(title)}</h1>
{para('Theory: this chapter starts from the beginner concept, then connects it to the concrete C++ and SDL2 implementation used in Jumpa Verse.')}
{para('Concept: the module is built as a small real-time system where state is stored in C++ objects, updated once per frame, and drawn through SDL2 renderer commands.')}
{para('Implementation: the relevant source surface for this chapter is ' + file_summary + '. The code favors simple classes, vectors, SDL_Rect geometry, shared SDL_Texture pointers, and explicit event polling.')}
"""
    body = [intro]
    for p, cap, exp in figures:
        body.append(fig_html(p, cap, exp))
    body.append(special)
    body.append("<h2>Concepts Before Code</h2>")
    body.append(para("C++ concepts used here include classes, constructors, references, pointers, enums, namespaces, stack variables, dynamic containers, and RAII-style cleanup where SDL objects are manually destroyed. STL concepts include std::vector storage, push/emplace operations, erase-based recycling, std::string text state, fstream streams, sorting, and iterating by index or reference. SDL2 concepts include renderer ownership, event queues, rectangles as geometry, texture resources, surfaces as temporary CPU bitmaps, and frame presentation."))
    body.append("<h2>Implementation Notes</h2>")
    body.append(para(theme))
    for file_name in files[:2]:
        excerpt = read_file(file_name, 70)
        if excerpt:
            body.append(f"<h3>Code Excerpt: {html.escape(file_name)}</h3>")
            body.append(code_block(excerpt))
            body.append(para("Execution flow: the snippet participates in the game loop by reading current state, calculating next state, then issuing render or storage operations. Memory flow: SDL resources are referenced by pointer, while gameplay objects and rectangles are copied or moved inside vectors. Object interaction: main.cpp coordinates these calls so each subsystem remains small and testable by inspection."))
    body.append("<h2>Issues Faced and Solutions</h2>")
    body.append(para("Typical issues in this module include invalid SDL pointers, oversized coordinates, shallow-copy confusion for textures, repeated text texture creation, off-screen objects growing memory, and input state transitions. The implemented solution pattern is to validate pointers, clamp screen-dependent values, share texture pointers without ownership, erase off-screen objects, cap deltaTime, and centralize screen-size queries."))
    return "\n".join(body)


def build():
    FIG.mkdir(parents=True, exist_ok=True)

    fig_counter = []
    def make(n, title, kind, items=None, accent=None):
        p = save(n, title, kind, items, accent)
        fig_counter.append((p, title))
        return p

    chapters = []
    chapters.append(chapter(1, "INTRODUCTION TO THE GAME",
        "Jumpa Verse is a side-scrolling survival platform game. The player is a rectangle-like avatar that jumps over procedurally placed spike traps while the world scrolls left. Death occurs when the player falls below the visible play field or intersects a trap hitbox.",
        ["main.cpp", "object.h"],
        [
            (make("ch01_architecture.png", "Game Architecture Overview", "architecture"), "Figure 1.1 Game architecture diagram", "The architecture places main.cpp as the coordinator between SDL2, gameplay entities, screen states, and asset files."),
            (make("ch01_gameplay_flow.png", "Gameplay Flowchart", "flow", ["Open Menu", "Login", "Reset Gameplay", "Poll Events", "Physics", "Collision", "Render", "Game Over"]), "Figure 1.2 Gameplay flowchart", "This flow matches the nested menu loop and active gameplay loop in main.cpp."),
            (make("ch01_loop.png", "Real-Time Game Loop", "pipeline"), "Figure 1.3 Main game loop diagram", "Each frame moves from event input to update logic to rendering and presentation."),
            (make("ch01_state.png", "Character State Diagram", "flow", ["Idle on ground", "Jump pressed", "Airborne", "Gravity falling", "Lands or dies"]), "Figure 1.4 Character state diagram", "The player state is expressed by onGround, jumpVelocity, gravity, and collision checks."),
        ]))

    chapters.append(chapter(2, "COLOR SCHEME, UI STYLE, FONT SYSTEM",
        "The UI uses a high-contrast dark background, orange highlight, silver inactive buttons, and white text. The custom Debrosee typeface gives title identity, while system fonts are used as numeric fallbacks so integer scores remain readable.",
        ["menuScreen.cpp", "score.cpp"],
        [
            (make("ch02_palette.png", "Color Palette and Contrast", "chart", ["orange", "steel", "dark", "white", "silver", "brown"], COLORS["orange"]), "Figure 2.1 Color palette figure", "Orange is the action color; dark gray is the stable background; white and silver maintain readability."),
            (make("ch02_menu_mockup.png", "Main Menu UI Mockup", "ui"), "Figure 2.2 UI preview layout", "The mockup mirrors menuScreen.cpp with title, Play/Scores/Exit buttons, ground line, cube, and animated trap preview."),
            (make("ch02_typography.png", "Typography Role Comparison", "flow", ["Title font", "Button labels", "Score label", "Digit fallback", "Message text"]), "Figure 2.3 Typography comparison", "Different text roles are handled by TTF_OpenFont and text-to-texture rendering."),
        ]))

    chapters.append(chapter(3, "TEXTURE SYSTEM AND GROUND TEXTURING",
        "Ground rendering divides each platform into SDL_Rect tiles. The top row uses ground.jpeg and the lower rows use underGround.jpeg. Textures are loaded once in main.cpp and shared by pointer to avoid expensive duplicated GPU allocations.",
        ["object.h", "main.cpp"],
        [
            (make("ch03_atlas.png", "Ground Texture Tiling Atlas", "geometry", accent=COLORS["green"]), "Figure 3.1 Texture atlas and tiling visual", "The platform surface is repeated with SDL_RenderCopy into individual destination rectangles."),
            (make("ch03_memory.png", "Texture Pointer Memory Model", "memory", ["ground vector", "ground_Class value", "SDL_Rect tile arrays", "shared SDL_Texture*"]), "Figure 3.2 Texture memory layout", "The diagram separates object geometry from shared renderer-owned texture resources."),
            (make("ch03_pipeline.png", "Texture Rendering Pipeline", "pipeline", ["JPEG file", "IMG_LoadTexture", "SDL_Texture", "SDL_Rect targets", "SDL_RenderCopy"]), "Figure 3.3 Rendering flow diagram", "Texture pixels move from image files into GPU textures, then are copied to rectangles every frame."),
            (make("ch03_uml.png", "Texture and Ground UML", "uml", accent=COLORS["green"]), "Figure 3.4 UML diagram for texture-ground relation", "ground_Class composes rectangle vectors and associates with shared SDL_Texture pointers."),
        ],
        sdl_function_table()))

    chapters.append(chapter(4, "GROUND GENERATION SYSTEM",
        "Ground generation uses std::random_device, std::mt19937, and uniform distributions to create platform width, height, gap, and y-position. Existing platforms are recycled by erasing objects that leave the left side of the screen.",
        ["ground.cpp", "object.h"],
        [
            (make("ch04_lifecycle.png", "Ground Lifecycle", "flow", ["Initial ground", "Generate next", "Classify size", "Spawn traps", "Camera shifts left", "Erase off-screen"]), "Figure 4.1 Ground lifecycle diagram", "The lifecycle directly follows ground_generator, groundRandomgenerator, cameraMovmentObj, and freeMemory."),
            (make("ch04_vector.png", "Vector Storage and Recycling", "memory", ["std::vector<ground_Class>", "emplace_back", "contiguous objects", "erase old left-side objects"]), "Figure 4.2 Vector memory diagram", "The vector stores platform objects by value and shrinks as old platforms are removed."),
            (make("ch04_sequence.png", "Ground Generation Sequence", "pipeline", ["main loop", "buffer check", "groundRandomgenerator", "trapTriangle", "render"]), "Figure 4.3 Sequence diagram", "main.cpp generates new platforms when the last platform does not extend far enough ahead."),
        ]))

    chapters.append(chapter(5, "CAMERA SYSTEM",
        "SDL2 does not provide a game camera. Jumpa Verse simulates one by shifting world objects left using camera offsets while the player remains visually near the same screen region.",
        ["camera.cpp", "main.cpp"],
        [
            (make("ch05_illusion.png", "Camera Illusion", "pipeline", ["world x", "offset", "screen x", "render rect", "visible motion"], COLORS["blue"]), "Figure 5.1 Camera illusion diagram", "The side-scrolling effect is created by changing object coordinates before rendering."),
            (make("ch05_before_after.png", "Before and After Offset", "geometry", accent=COLORS["blue"]), "Figure 5.2 Side scrolling figure", "Ground and traps shift together, preserving relative positions."),
            (make("ch05_conversion.png", "Coordinate Conversion", "flow", ["read cameraOffset", "cast to int", "add to ground.x", "rebuild tile rects", "add to trap.x"]), "Figure 5.3 Coordinate conversion visual", "cameraMovmentObj updates platform geometry and trap x positions in the same pass."),
        ]))

    chapters.append(chapter(6, "TRAP SYSTEM",
        "Traps are spike triangles rendered with SDL2_gfx. Spawn probability is bounded by attempts, platform width margins, maximum trap count, and minimum spacing so traps do not overlap excessively or create infinite generation loops.",
        ["traps.cpp", "object.h"],
        [
            (make("ch06_collision.png", "Trap Collision and Hitbox", "geometry", accent=COLORS["red"]), "Figure 6.1 Trap collision diagram", "The visible triangle is approximated by a rectangle for SDL_HasIntersection collision checks."),
            (make("ch06_probability.png", "Trap Probability and Attempts", "chart", ["spawn", "margin", "spacing", "attempt", "skip", "place"], COLORS["red"]), "Figure 6.2 Spawn probability chart", "Each attempt has a probability gate, and the loop stops after maxAttempts to avoid runaway logic."),
            (make("ch06_lifecycle.png", "Trap Lifecycle", "flow", ["platform selected", "probability gate", "spacing check", "emplace spike", "render triangle", "free off-screen"]), "Figure 6.3 Trap lifecycle flowchart", "The trap system is linked to platform generation and off-screen cleanup."),
            (make("ch06_uml.png", "Trap UML and Relationships", "uml", accent=COLORS["red"]), "Figure 6.4 Trap UML diagram", "trapSpike inherits hitBox and depends on SDL2_gfx rendering functions."),
        ],
        para("Specific fixes: trap overlap is reduced with minTrapSpacing; probability is controlled by chances(0,1000) < 100; stack overflow/infinite loop risk is controlled by maxAttempts; SDL2_gfx int16 overflow streaks are reduced by visibility checks in trapSpike::render; trap color line issues are handled by drawing a filled triangle first and a darker outline second.")))

    chapters.append(chapter(7, "MENU SYSTEM",
        "The menu is an interactive SDL screen that draws the title, buttons, animated preview objects, and custom window controls. It handles keyboard selection, mouse hover, click actions, and transitions to login, score menu, or exit.",
        ["menuScreen.cpp", "object.h"],
        [
            (make("ch07_ui.png", "Menu Screen Composition", "ui"), "Figure 7.1 Menu UI diagram", "The visible screen uses SDL_RenderFillRect, text textures, line drawing, and SDL2_gfx triangles."),
            (make("ch07_hover.png", "Button Hover State", "flow", ["mouse position", "point-in-rect", "selectedButton", "stateHolder", "rebuild text textures", "render"]), "Figure 7.2 Hover effect flow", "Hover state changes selected colors and text textures."),
            (make("ch07_state.png", "Menu State Transitions", "flow", ["Menu", "Login", "Register", "Scores", "Game", "Exit"]), "Figure 7.3 Menu state diagram", "The menu is the root navigation state for the application."),
        ]))

    chapters.append(chapter(8, "PLAYER SYSTEM AND MOVEMENT",
        "The player is a rectangle with velocity, gravity, jump power, and onGround state. Movement combines SDL keyboard state, mouse buttons, gravity integration, horizontal clamping, and top-only platform collision.",
        ["player.cpp", "object.h"],
        [
            (make("ch08_movement.png", "Player Movement Forces", "geometry", accent=COLORS["violet"]), "Figure 8.1 Movement diagram", "Jump velocity pushes up, gravity pulls down, and platform collision snaps the player onto the ground."),
            (make("ch08_physics.png", "Physics Flowchart", "flow", ["Read keyboard", "Read mouse", "apply jump", "apply gravity", "move x", "move y", "test ground", "sync hitbox"], COLORS["violet"]), "Figure 8.2 Physics flowchart", "physicsDevelop applies input before resolving vertical collision."),
            (make("ch08_state.png", "Player Boolean State", "flow", ["onGround true", "jump", "onGround false", "falling", "collision", "onGround true"], COLORS["violet"]), "Figure 8.3 Player state diagram", "The state cycle is controlled by onGround and jumpVelocity."),
        ]))

    chapters.append(chapter(9, "TEXT RENDERING ISSUES",
        "Text rendering in SDL_ttf is a two-step process: first rasterize a string to SDL_Surface, then upload it as SDL_Texture. The project uses font fallbacks for numeric score values because decorative fonts can make integers hard to read.",
        ["score.cpp", "loginScreen.cpp"],
        [
            (make("ch09_pipeline.png", "Text Rendering Pipeline", "pipeline", ["TTF_OpenFont", "TTF_RenderText", "SDL_Surface", "SDL_Texture", "SDL_RenderCopy"], COLORS["green"]), "Figure 9.1 Text rendering pipeline", "Every visible text label is converted into texture data before it can be copied by the renderer."),
            (make("ch09_font.png", "Font Fallback System", "flow", ["Debrosee", "LiberationSans", "DejaVuSans", "fallback passed font"]), "Figure 9.2 Font system visual", "The score system uses a digit fallback to solve integer rendering/readability issues."),
        ]))

    chapters.append(chapter(10, "ERROR HANDLING SYSTEM",
        "Initialization is defensive: SDL, SDL_ttf, SDL_image, window creation, and renderer creation are each checked. Failure returns false and main.cpp exits early instead of entering an invalid render loop.",
        ["Error_Handlers.cpp", "main.cpp"],
        [
            (make("ch10_init.png", "Initialization Flowchart", "flow", ["SDL_Init", "TTF_Init", "IMG_Init", "display mode", "create window", "create renderer"], COLORS["red"]), "Figure 10.1 Initialization flowchart", "Each initialization step must succeed before gameplay begins."),
            (make("ch10_error.png", "Error Handling Branches", "pipeline", ["call", "check return", "log SDL_GetError", "return false", "main exits"], COLORS["red"]), "Figure 10.2 Error handling diagram", "The system prevents null window or renderer usage."),
        ]))

    chapters.append(chapter(11, "LOGIN SCREEN AND GAME OVER SCREEN",
        "Login, registration, scores, and game over are separate screen loops. They use namespaces for helper functions, enums for actions, text input events for form fields, and CSV files for persistent player data.",
        ["loginScreen.cpp", "register.cpp", "gameover.cpp"],
        [
            (make("ch11_transition.png", "Screen Transitions", "flow", ["Menu", "Login", "Register", "Game", "Game Over", "Try Again/Menu"], COLORS["orange"]), "Figure 11.1 Screen transition diagram", "Screen action enums make transitions explicit and readable."),
            (make("ch11_namespace.png", "Namespace Helper Boundary", "uml", accent=COLORS["orange"]), "Figure 11.2 Namespace diagram", "Anonymous namespaces keep helper functions private to their translation units."),
        ]))

    chapters.append(chapter(12, "FILE HANDLING",
        "CSV persistence is intentionally beginner-friendly. ifstream reads existing players and scores, ofstream appends final scores, and fstream appends registered players. String streams parse comma-separated records.",
        ["loginScreen.cpp", "register.cpp", "gameover.cpp", "scoreMenu.cpp"],
        [
            (make("ch12_fileio.png", "File IO Data Flow", "pipeline", ["player.csv", "ifstream", "stringstream", "validate/sort", "ofstream/fstream"], COLORS["green"]), "Figure 12.1 File IO diagram", "The player file stores username,password,score history in comma-separated lines."),
            (make("ch12_readwrite.png", "Read Write System", "flow", ["open file", "read line", "split by comma", "compare username", "append score", "rewrite file"], COLORS["green"]), "Figure 12.2 Data flow visual", "The game over screen appends scores to the logged-in player row."),
        ]))

    chapters.append(chapter(13, "LOGIN/REGISTRATION ANIMATIONS",
        "The account screens use frame-by-frame redraws, hover highlights, active input borders, and message transitions. The animation model is simple: each loop handles events, updates visual state, clears, draws, presents, and delays.",
        ["loginScreen.cpp", "register.cpp"],
        [
            (make("ch13_timeline.png", "UI Animation Timeline", "pipeline", ["event", "hover", "active field", "draw input", "present"], COLORS["violet"]), "Figure 13.1 Animation timeline", "Interactive feedback is produced by redrawing selected states each frame."),
            (make("ch13_frames.png", "Frame Progression", "flow", ["Frame N", "mouse move", "selected changes", "Frame N+1", "new colors"], COLORS["violet"]), "Figure 13.2 Frame progression visual", "The user sees animation because the render loop keeps presenting updated states."),
        ]))

    chapters.append(chapter(14, "COMPLETE GAME FLOW",
        "The complete system starts at initialization, enters menu navigation, resets gameplay state, processes the real-time loop, saves death results, and returns to menu or restarts based on the selected action.",
        ["main.cpp", "functions.h"],
        [
            (make("ch14_master.png", "Master Architecture", "architecture", accent=COLORS["blue"]), "Figure 14.1 Master architecture diagram", "The full application is a set of screen and gameplay loops coordinated by main.cpp."),
            (make("ch14_sequence.png", "Complete Program Sequence", "pipeline", ["init", "menu", "login", "reset", "game loop", "game over", "close"], COLORS["blue"]), "Figure 14.2 Full sequence diagram", "This is the end-to-end program route from startup to shutdown."),
        ]))

    chapters.append(chapter(15, "RANDOM NUMBER GENERATION",
        "Randomness controls ground y-position, width, height, gap, and trap placement. The current implementation uses std::random_device and std::mt19937 for modern C++ random generation, while the chapter also explains beginner srand/rand concepts for comparison.",
        ["ground.cpp", "traps.cpp"],
        [
            (make("ch15_rng.png", "RNG Flowchart", "flow", ["seed source", "mt19937 engine", "distribution", "candidate value", "constraint check"], COLORS["orange"]), "Figure 15.1 RNG flowchart", "The engine and distribution split seed generation from number range logic."),
            (make("ch15_prob.png", "Probability Systems", "chart", ["ground y", "ground w", "height", "gap", "trap chance", "attempts"], COLORS["orange"]), "Figure 15.2 Probability visual", "Different random variables shape difficulty and platform rhythm."),
        ]))

    chapters.append(chapter(16, "LIBRARIES USED",
        "The project includes SDL core headers, SDL_render, SDL_video, SDL_image, SDL_ttf, SDL2_gfx, iostream, iterator, vector, string/string.h, algorithm, stdlib, fstream, sstream, and project headers. Each library contributes a narrow role.",
        ["main.cpp", "functions.h"],
        [
            (make("ch16_deps.png", "Library Dependency Diagram", "architecture", accent=COLORS["green"]), "Figure 16.1 Library dependency diagram", "Project modules depend on SDL extensions and standard C++ libraries."),
            (make("ch16_roles.png", "Header Role Map", "flow", ["SDL.h events", "SDL_render draw", "SDL_image textures", "SDL_ttf text", "vector storage", "fstream CSV"], COLORS["green"]), "Figure 16.2 Header role map", "Headers map directly to rendering, input, text, storage, and containers."),
        ],
        sdl_function_table()))

    chapters.append(chapter(17, "FULLSCREEN IMPLEMENTATION",
        "Fullscreen mode is enabled at window creation with SDL_WINDOW_FULLSCREEN_DESKTOP and toggled later through custom window controls using SDL_SetWindowFullscreen. Windowed mode restores the configured GAME_WIDTH and GAME_HEIGHT.",
        ["Error_Handlers.cpp", "functions.h"],
        [
            (make("ch17_fullscreen.png", "Fullscreen Toggle Flow", "flow", ["click custom icon", "SDL_GetWindowFlags", "if fullscreen", "set windowed", "else fullscreen desktop"], COLORS["blue"]), "Figure 17.1 Fullscreen diagram", "The toggle checks current flags before changing the window mode."),
            (make("ch17_resolution.png", "Resolution Comparison", "chart", ["960x540", "1980x1260", "desktop", "4K scale", "tile scale", "trap scale"], COLORS["blue"]), "Figure 17.2 Resolution comparison", "Screen-dependent scaling functions keep gameplay readable across display sizes."),
        ]))

    chapters.append(chapter(18, "CLAMPING SYSTEM",
        "Clamping restricts values to safe ranges. The project clamps generation ranges, player x-position, deltaTime, tile sizes, and screen-dependent platform dimensions so random generation and physics remain playable.",
        ["ground.cpp", "player.cpp", "main.cpp"],
        [
            (make("ch18_clamp.png", "Coordinate Limits", "geometry", accent=COLORS["red"]), "Figure 18.1 Coordinate limit diagram", "Clamping prevents positions and dimensions from leaving useful gameplay boundaries."),
            (make("ch18_boundary.png", "Boundary Control Flow", "flow", ["candidate value", "min check", "max check", "safe value", "use in gameplay"], COLORS["red"]), "Figure 18.2 Boundary visual", "The clampInt helper in ground.cpp is representative of this safety pattern."),
        ]))

    chapters.append(chapter(19, "MAIN.CPP COMPLETE FLOW",
        "main.cpp owns the startup, gameplay reset lambda, menu gate, per-frame timing, event polling, physics calls, collision/death checks, score update, generation buffer, cleanup, camera movement, rendering, FPS throttling, and shutdown.",
        ["main.cpp"],
        [
            (make("ch19_mainloop.png", "Main.cpp Execution Flow", "flow", ["declare SDL pointers", "init", "load textures/fonts", "resetGameplay", "menu loop", "game loop", "close"], COLORS["slate"]), "Figure 19.1 Main loop diagram", "The line-by-line structure of main.cpp is a staged real-time application lifecycle."),
            (make("ch19_cycles.png", "Physics and Rendering Cycle", "pipeline", ["deltaTime", "input", "physics", "collision", "generation", "camera", "render"], COLORS["slate"]), "Figure 19.2 Execution flowchart", "Each frame repeats the same update and draw order to keep behavior deterministic enough for gameplay."),
        ],
        para("Line-by-line summary: SDL pointers are initialized to null; player and gameplay containers are created; init() validates subsystems; textures and fonts are loaded; resetGameplay clears containers and recreates the starting world; the menu loop blocks gameplay until Play/Login succeeds; deltaTime is capped; events are polled; keyboard state drives physicsDevelop; death checks trigger GameOver; score updates only while alive; rendering clears the sky, generates enough future ground, frees old objects, applies camera movement, renders ground/traps/player/score/window controls, presents the frame, delays to target 60 FPS, and finally close() destroys SDL resources.")))

    toc = "".join(f"<li>Chapter {i} - {title}</li>" for i, title in [
        (1,"INTRODUCTION TO THE GAME"),(2,"COLOR SCHEME, UI STYLE, FONT SYSTEM"),(3,"TEXTURE SYSTEM AND GROUND TEXTURING"),
        (4,"GROUND GENERATION SYSTEM"),(5,"CAMERA SYSTEM"),(6,"TRAP SYSTEM"),(7,"MENU SYSTEM"),(8,"PLAYER SYSTEM AND MOVEMENT"),
        (9,"TEXT RENDERING ISSUES"),(10,"ERROR HANDLING SYSTEM"),(11,"LOGIN SCREEN AND GAME OVER SCREEN"),(12,"FILE HANDLING"),
        (13,"LOGIN/REGISTRATION ANIMATIONS"),(14,"COMPLETE GAME FLOW"),(15,"RANDOM NUMBER GENERATION"),(16,"LIBRARIES USED"),
        (17,"FULLSCREEN IMPLEMENTATION"),(18,"CLAMPING SYSTEM"),(19,"MAIN.CPP COMPLETE FLOW")])
    figs = "".join(f"<li>Figure {i+1}: {html.escape(title)}</li>" for i, (_, title) in enumerate(fig_counter))

    css = """
@page { size: A4; margin: 0.72in; }
body { font-family: Arial, Helvetica, sans-serif; color: #181c24; line-height: 1.42; font-size: 10.5pt; }
.cover { text-align:center; padding-top: 120px; page-break-after: always; }
.cover h1 { font-size: 34pt; color: #ff7f00; margin-bottom: 10px; }
.cover h2 { font-size: 18pt; color: #2b333f; }
h1 { font-size: 21pt; color: #1f2a37; border-bottom: 3px solid #ff7f00; padding-bottom: 8px; page-break-before: always; }
h2 { font-size: 15pt; color: #2f5f93; margin-top: 20px; }
h3 { font-size: 12.5pt; color: #384354; }
p { margin: 7px 0 10px 0; text-align: justify; }
figure { margin: 18px auto 20px auto; text-align: center; page-break-inside: avoid; }
figure img { width: 4.85in; height: auto; max-width: 4.85in; max-height: 2.91in; object-fit: contain; border: 1px solid #cfd8e3; }
figcaption { font-weight: bold; color: #2b333f; margin-top: 6px; }
.figure-note { font-size: 9.5pt; color: #4f5f70; text-align: center; }
pre { background: #fff3e0; color: #b45309; padding: 12px; border-left: 5px solid #ff7f00; border-top: 1px solid #ffd7a8; border-bottom: 1px solid #ffd7a8; font-family: Consolas, 'Courier New', monospace; font-size: 8.2pt; white-space: pre-wrap; }
pre code { color: #b45309; background: #fff3e0; }
code { font-family: Consolas, 'Courier New', monospace; color: #c45a00; background: #fff7ed; }
table { border-collapse: collapse; width: 100%; margin: 14px 0; page-break-inside: avoid; }
caption { font-weight: bold; margin-bottom: 6px; color: #2b333f; }
th { background: #2b333f; color: white; }
td, th { border: 1px solid #cfd8e3; padding: 7px; vertical-align: top; }
.front { page-break-after: always; }
ul { margin-top: 6px; }
"""

    html_doc = f"""<!doctype html>
<html>
<head><meta charset="utf-8"><title>Jumpa Verse SDL2 C++ Documentation</title><style>{css}</style></head>
<body>
<section class="cover">
<h1>Jumpa Verse</h1>
<h2>Complete Premium SDL2 + C++ Game Documentation</h2>
<p style="text-align:center;margin-top:60px;">Professional software engineering thesis, UML guide, beginner handbook, and technical game development documentation.</p>
<p style="text-align:center;">Generated from the actual project source files in <b>{html.escape(str(ROOT))}</b>.</p>
</section>
<section class="front"><h1>Abstract</h1>{para('This document explains Jumpa Verse, a 2D SDL2 and C++ side-scrolling platform game, from theory to implementation. It covers architecture, rendering, texture handling, procedural ground generation, camera illusion, trap spawning, menu screens, login and registration, score storage, fullscreen behavior, clamping, random generation, and the complete main loop. The report is written for beginners while preserving professional software architecture detail.')}</section>
<section class="front"><h1>Acknowledgement</h1>{para('This report acknowledges the SDL2 ecosystem, the C++ standard library, and the project implementation effort represented by the Jumpa Verse source files. The explanations are designed to help a beginner understand the codebase and also help an evaluator inspect the software engineering decisions.')}</section>
<section class="front"><h1>Table of Contents</h1><ol>{toc}</ol></section>
<section class="front"><h1>List of Figures</h1><ol>{figs}</ol></section>
<section class="front"><h1>List of Tables</h1><ol><li>SDL2 and extension functions used by Jumpa Verse</li><li>Library roles and project responsibilities</li></ol></section>
{''.join(chapters)}
<h1>Bibliography</h1>
<p>[1] SDL2 Documentation. [2] SDL_image Documentation. [3] SDL_ttf Documentation. [4] SDL2_gfx Primitive Documentation. [5] ISO C++ Standard Library references for vector, string, fstream, algorithm, and random.</p>
<h1>Appendix A - UML Appendix</h1>
{fig_html(make('appendix_uml_master.png', 'Complete UML Appendix Diagram', 'uml'), 'Appendix Figure A.1 Complete UML appendix', 'This consolidated UML-style diagram summarizes inheritance, association, and dependency relationships used throughout the game.')}
<h1>Appendix B - Code Appendix</h1>
<h2>main.cpp</h2>{code_block(read_file('main.cpp', 260))}
<h2>object.h</h2>{code_block(read_file('object.h', 260))}
<h2>ground.cpp</h2>{code_block(read_file('ground.cpp', 220))}
<h2>player.cpp</h2>{code_block(read_file('player.cpp', 180))}
<h2>traps.cpp</h2>{code_block(read_file('traps.cpp', 160))}
</body></html>
"""
    HTML_OUT.write_text(html_doc, encoding="utf-8")
    print(HTML_OUT)


if __name__ == "__main__":
    build()
