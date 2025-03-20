#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

enum stmts
{
	start,
	add,
	sub,
	mult,
	move,
	turn,
	set,
	let,
	whilehead,
	whiletail,
	ifhead,
	iftail
};

struct draw
{
	enum stmts stmts;
	Rectangle rec;
	int xoffset;
	int yoffset;
	Rectangle collisionRec;
	Color clr;
	char *str;
	Color strClr;
};

struct list
{
	struct draw draw;
	struct list *next;
};

struct list * insertNext(struct list *list, enum stmts stmt, Font fnt)
{
	struct draw tempDraw;
	
	tempDraw.xoffset = list->draw.xoffset;
	tempDraw.yoffset = list->draw.yoffset;
	tempDraw.stmts = stmt;
	
	char *tempStr;
	switch(stmt)
	{
		case add: tempStr = "add 5 to i"; break;
		case sub: tempStr = "sub 5 from i"; break;
		case mult: tempStr = "multiply i by 5"; break;
		case move: tempStr = "move turtle forward i px"; break;
		case turn: tempStr = "turn turtle left i degrees"; break;
		case set: tempStr = "set i equal to 5"; break;
		case let: tempStr = "let i equal 5"; break;
		case whilehead: tempStr = "while i islessthan 5 do"; break;
		case whiletail: tempStr = "end while"; break;
		case ifhead: tempStr = "if i islessthan 5 then"; break;
		case iftail: tempStr = "end if"; break;
	}
	
	Vector2 size = MeasureTextEx(fnt, tempStr, 16, 1);
	tempDraw.rec.width = size.x+5;
	tempDraw.rec.height = size.y+5;
	
	tempDraw.clr = BLUE;
	tempDraw.strClr = WHITE;
	
	tempDraw.collisionRec.width = tempDraw.rec.width;
	tempDraw.collisionRec.height = tempDraw.rec.height-2;
	
	tempDraw.str = tempStr;
	
	struct list *tmpNode;
	if(list->next != NULL)
	{
		tmpNode = malloc(sizeof(struct list));
	    *tmpNode = *(list->next);
	} else
	{
	    tmpNode = NULL;
	}
	
	list->next = realloc(list->next, sizeof(struct list));
    list->next->draw = tempDraw;
    list->next->next = tmpNode;
    
    return list;
}

struct list * setOffset(struct list *list)
{
	int xoffset = 0, yoffset = 0;
	
	struct list *tmp;
	for(tmp = list->next; tmp != NULL; tmp = tmp->next)
	{
		if(tmp->draw.stmts == whiletail ||
		   tmp->draw.stmts == iftail) xoffset -= 20;
	
		tmp->draw.rec.x = xoffset;
		tmp->draw.rec.y = yoffset;
		
		tmp->draw.collisionRec.x = tmp->draw.rec.x;
		tmp->draw.collisionRec.y = tmp->draw.rec.y+10;
		
		if(tmp->draw.stmts == whilehead ||
		   tmp->draw.stmts == ifhead) xoffset += 20;
		
		yoffset += 20;
	}
	
	return list;
}

struct list * deleteNext(struct list *list)
{
    struct list *tmpNode = list->next;
    list->next = list->next->next;
    free(tmpNode);
    
    return list;
}

void drawVals(struct list *list, Font fnt)
{
	struct list *tmp;
	for(tmp = list->next; tmp != NULL; tmp = tmp->next)
	{
		Vector2 strPos = {tmp->draw.rec.x+2, tmp->draw.rec.y+2};
		DrawRectangleRec(tmp->draw.rec, tmp->draw.clr);
		DrawTextEx(fnt, tmp->draw.str, strPos, 16, 1, tmp->draw.strClr);
	}
}

struct btns
{
	char *str;
	Rectangle box;
	int type;
};

struct btns * createButtons(struct btns *buttons, Font fnt)
{
	char *str[9] = {"add", "sub", "mult", "move", "turn", "set", "let", "while", "if"};
	
	int xoffset = 0, yoffset = 300;
	
	int i;
	for(i = 0; i<9; i++)
	{
		buttons[i].type = i;
		
		Vector2 size = MeasureTextEx(fnt, str[i], 16, 1);
		int width = size.x+5;
		int height = size.y+5;
		
		buttons[i].box.x = xoffset;
		buttons[i].box.y = yoffset;
		buttons[i].box.width = width;
		buttons[i].box.height = height;
		
		buttons[i].str = str[i];
		
		yoffset += 20;
	}
	
	return buttons;
}

void drawButtons(struct btns *buttons, Font fnt)
{
	int i;
	for(i = 0; i<9; i++)
	{
		Vector2 strPos = {buttons[i].box.x+2, buttons[i].box.y+2};
		DrawRectangleRec(buttons[i].box, BLUE);
		DrawTextEx(fnt, buttons[i].str, strPos, 16, 1, WHITE);
	}
}

int main(void)
{
	struct list *list = malloc(sizeof(struct list));
	list->draw.stmts = start;
	list->draw.str = NULL;
	list->next = NULL;
	list->draw.xoffset = 0;
	list->draw.yoffset = 0;
	list->draw.collisionRec.x = 0;
	list->draw.collisionRec.y = 0;
	list->draw.collisionRec.width = 100;
	list->draw.collisionRec.height = 20;
	
	struct btns *buttons = malloc(9 * sizeof(struct btns));
	
	InitWindow(500, 500, "test");
	
	SetTargetFPS(60);
	
	Font unifont = LoadFont("unifont.otf");
	buttons = createButtons(buttons, unifont);
	
	enum stmts stmt;
	int whilecond = 0, ifcond = 0;
	int enable = 0;
	
	while(!WindowShouldClose())
	{
		BeginDrawing();
		
		DrawRectangleRec(list->draw.collisionRec, GREEN);
		
		drawVals(list, unifont);
		drawButtons(buttons, unifont);
		
		Vector2 msPos = GetMousePosition();
		
		int i;
		for(i = 0; i<9; i++)
		{
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, buttons[i].box))
			{
				switch(buttons[i].type)
				{
					case 0: stmt = add; break;
					case 1: stmt = sub; break;
					case 2: stmt = mult; break;
					case 3: stmt = move; break;
					case 4: stmt = turn; break;
					case 5: stmt = set; break;
					case 6: stmt = let; break;
					case 7: whilecond = 1; break;
					case 8: ifcond = 1; break;
				}
				enable = 1;
			}
		}
		
		if(enable == 1)
		{
			/* draw green line below instructions */
			struct list *tmp;
			for(tmp = list->next; tmp != NULL; tmp = tmp->next)
			{
				if(CheckCollisionPointRec(msPos, tmp->draw.collisionRec))
				{
					DrawRectangle(tmp->draw.rec.x, tmp->draw.rec.y+18, tmp->draw.rec.width, 2, GREEN);
				}
			}
		
			Rectangle tempRec = {msPos.x, msPos.y, 50, 50};
			DrawRectangleRec(tempRec, RED);
		}
		
		struct list *tmp;
		for(tmp = list; tmp != NULL; tmp = tmp->next)
		{
			if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmp->draw.collisionRec) && enable == 1)
			{
				if(whilecond == 1)
				{
					tmp = insertNext(tmp, whiletail, unifont);
					tmp = insertNext(tmp, whilehead, unifont);
					whilecond = 0;
				} else if(ifcond == 1)
				{
					tmp = insertNext(tmp, ifhead, unifont);
					ifcond = 0;
				} else
				{
					tmp = insertNext(tmp, stmt, unifont);
				}
				list = setOffset(list);
				enable = 0;
			}
		}
		
		ClearBackground(RAYWHITE);
		EndDrawing();
	}
	
	return 0;
}