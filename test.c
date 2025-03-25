#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	char *optVar1;
	Vector2 optVar1Pos;
	char *optVar2;
	Vector2 optVar2Pos;
	char *optCond;
	Vector2 optCondPos;
};

struct list
{
	struct draw draw;
	struct list *next;
};

struct list * insertNext(struct list *list, enum stmts stmt, Font fnt)
{
	struct draw tempDraw;
	tempDraw.stmts = stmt;
	
	tempDraw.optVar1 = " ";
	tempDraw.optVar2 = " ";
	tempDraw.optCond = " ";
	
	char *tempStr = calloc(1, 1);
	int totalSize;
	switch(stmt)
	{
		case add:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optVar2) + 9;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "add ");
			strcat(tempStr, tempDraw.optVar1);
			tempDraw.optVar1Pos = MeasureTextEx(fnt, tempStr, 16, 1);
			strcat(tempStr, " to ");
			strcat(tempStr, tempDraw.optVar2);
			tempDraw.optVar2Pos = MeasureTextEx(fnt, tempStr, 16, 1);
		break;
		
		case sub:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optVar2) + 11;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "sub ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, " from ");
			strcat(tempStr, tempDraw.optVar2);
		break;
		
		case mult:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optVar2) + 14;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "multiply ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, " by ");
			strcat(tempStr, tempDraw.optVar2);
		break;
		
		case move:
			totalSize = strlen(tempDraw.optVar1) + 24;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "move turtle forward ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, " px");
		break;
		
		case turn: tempStr = "turn turtle left i degrees"; break;
		
		case set:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optVar2) + 15;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "set ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, " equal to ");
			strcat(tempStr, tempDraw.optVar2);
		break;
		
		case let:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optVar2) + 12;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "let ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, " equal ");
			strcat(tempStr, tempDraw.optVar2);
		break;
		
		case whilehead:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optCond) + strlen(tempDraw.optVar2) + 10;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "while ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, tempDraw.optCond);
			strcat(tempStr, tempDraw.optVar2);
			strcat(tempStr, " do");
		break;
		
		case whiletail: tempStr = "end while"; break;
		
		case ifhead:
			totalSize = strlen(tempDraw.optVar1) + strlen(tempDraw.optCond) + strlen(tempDraw.optVar2) + 9;
			tempStr = realloc(tempStr, totalSize);
			strcat(tempStr, "if ");
			strcat(tempStr, tempDraw.optVar1);
			strcat(tempStr, tempDraw.optCond);
			strcat(tempStr, tempDraw.optVar2);
			strcat(tempStr, " then");
		break;
		
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
		   tmp->draw.stmts == iftail)
		{
			xoffset -= 20;	
			tmp->draw.optVar1Pos.x -= 20;
			tmp->draw.optVar2Pos.x -= 20;	
			tmp->draw.optCondPos.x -= 20;
		}
		
		tmp->draw.rec.x = xoffset;
		tmp->draw.rec.y = yoffset;
		
		tmp->draw.collisionRec.x = tmp->draw.rec.x;
		tmp->draw.collisionRec.y = tmp->draw.rec.y+10;
		
		if(tmp->draw.stmts == whilehead ||
		   tmp->draw.stmts == ifhead)
		{
			xoffset += 20;
			tmp->draw.optVar1Pos.x += 20;
			tmp->draw.optVar2Pos.x += 20;	
			tmp->draw.optCondPos.x += 20;
		}
		
		yoffset += 20;
		tmp->draw.optVar1Pos.y += 20;
		tmp->draw.optVar2Pos.y += 20;
		tmp->draw.optCondPos.y += 20;
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
		//DrawRec(tmp->draw.optVar1Pos.x, tmp->draw.optVar1Pos.y, 8, RED);
		//DrawCircle(tmp->draw.optVar2Pos.x, tmp->draw.optVar2Pos.y, 8, RED);
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

struct vars
{
	char *str;
	Vector2 pos;
	int rad;
	int type;
};

struct vars * createVariables(struct vars *variables, Font fnt)
{
	char *str[9] = {"a", "b", "c", "d", "e", "f", "g", "h", "i"};
	
	int xoffset = 100, yoffset = 308;
	
	int i;
	for(i = 0; i<9; i++)
	{
		variables[i].type = i;
		
		variables[i].pos.x = xoffset;
		variables[i].pos.y = yoffset;
		variables[i].rad = 8;
		
		variables[i].str = str[i];
		
		yoffset += 16;
	}
	
	return variables;
}

void drawVariables(struct vars *variables, Font fnt)
{
	int i;
	for(i = 0; i<9; i++)
	{
		Vector2 strPos = {variables[i].pos.x-4, variables[i].pos.y-9};
		DrawCircle(variables[i].pos.x, variables[i].pos.y, (float)variables[i].rad, ORANGE);
		DrawTextEx(fnt, variables[i].str, strPos, 16, 1, WHITE);
	}
}

struct conds
{
	char *str;
	Rectangle box;
	int type;
};

struct conds * createConditionals(struct conds *conditionals, Font fnt)
{
	char *str[3] = {"islessthan", "isgreaterthan", "isequalto"};
	
	int xoffset = 150, yoffset = 300;
	
	int i;
	for(i = 0; i<3; i++)
	{
		conditionals[i].type = i;
		
		Vector2 size = MeasureTextEx(fnt, str[i], 16, 1);
		int width = size.x+5;
		int height = size.y+5;
		
		conditionals[i].box.x = xoffset;
		conditionals[i].box.y = yoffset;
		conditionals[i].box.width = width;
		conditionals[i].box.height = height;
		
		conditionals[i].str = str[i];
		
		yoffset += 20;
	}
	
	return conditionals;
}

void drawConditionals(struct conds *conditionals, Font fnt)
{
	int i;
	for(i = 0; i<3; i++)
	{
		Vector2 strPos = {conditionals[i].box.x+2, conditionals[i].box.y+2};
		DrawRectangleRec(conditionals[i].box, PURPLE);
		DrawTextEx(fnt, conditionals[i].str, strPos, 16, 1, WHITE);
	}
}

struct dirs
{
	char *str;
	Rectangle box;
	int type;
};

struct dirs * createDirections(struct dirs *directions, Font fnt)
{
	char *str[4] = {"forward", "backward", "left", "right"};
	
	int xoffset = 300, yoffset = 300;
	
	int i;
	for(i = 0; i<4; i++)
	{
		directions[i].type = i;
		
		Vector2 size = MeasureTextEx(fnt, str[i], 16, 1);
		int width = size.x+5;
		int height = size.y+5;
		
		directions[i].box.x = xoffset;
		directions[i].box.y = yoffset;
		directions[i].box.width = width;
		directions[i].box.height = height;
		
		directions[i].str = str[i];
		
		yoffset += 20;
	}
	
	return directions;
}

void drawDirections(struct dirs *directions, Font fnt)
{
	int i;
	for(i = 0; i<4; i++)
	{
		Vector2 strPos = {directions[i].box.x+2, directions[i].box.y+2};
		DrawRectangleRec(directions[i].box, RED);
		DrawTextEx(fnt, directions[i].str, strPos, 16, 1, WHITE);
	}
}

int hover(Vector2 msPos, struct list *list)
{
	/* draw green line below instructions */
	struct list *tmp;
	for(tmp = list->next; tmp != NULL; tmp = tmp->next)
	{
		if(CheckCollisionPointRec(msPos, tmp->draw.collisionRec))
		{
			DrawRectangle(tmp->draw.rec.x, tmp->draw.rec.y+18, tmp->draw.rec.width, 2, GREEN);
			return 1;
		}
	}
	
	/* draw red line when above start */
	if(CheckCollisionPointRec(msPos, list->draw.collisionRec))
	{
		if(list->next != NULL)
		{
			DrawRectangle(list->draw.collisionRec.x, list->draw.collisionRec.y, list->next->draw.collisionRec.width, 2, RED);
		} else
		{
			DrawRectangle(list->draw.collisionRec.x, list->draw.collisionRec.y, list->draw.collisionRec.width, 2, RED);
		}
		
		return 1;
	}
	
	return 0;
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
	list->draw.collisionRec.height = 10;
	
	struct btns *buttons = malloc(9 * sizeof(struct btns));
	struct vars *variables = malloc(9 * sizeof(struct vars));
	struct conds *conditionals = malloc(3 * sizeof(struct conds));
	struct dirs *directions = malloc(3 * sizeof(struct dirs));
	
	InitWindow(500, 500, "test");
	
	SetTargetFPS(60);
	
	Font unifont = LoadFont("unifont.otf");
	buttons = createButtons(buttons, unifont);
	variables = createVariables(variables, unifont);
	conditionals = createConditionals(conditionals, unifont);
	directions = createDirections(directions, unifont);
	
	enum stmts stmt;
	int whilecond = 0, ifcond = 0;
	
	int btnEnable = 0, movEnable = 0;
	
	int saveInst; /* save button instruction for displaying later */

	struct list *save = NULL;
	int blockLength = 0;
	
	while(!WindowShouldClose())
	{
		BeginDrawing();
		
		//DrawRectangleRec(list->draw.collisionRec, GREEN);
		
		drawVals(list, unifont);
		drawButtons(buttons, unifont);
		drawVariables(variables, unifont);
		drawConditionals(conditionals, unifont);
		drawDirections(directions, unifont);
		
		Vector2 msPos = GetMousePosition();
		
		/* check if a button is pressed */
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
				
				saveInst = i;
				btnEnable = 1;
			}
		}
		
		
		/* if a button has been pressed, enable it */
		if(btnEnable == 1)
		{
			hover(msPos, list);
		
			Vector2 tmpSize = MeasureTextEx(unifont, buttons[saveInst].str, 16, 1);
			int width = tmpSize.x+5;
			int height = tmpSize.y+5;
			
			/* rectangle position and drawing */
			Rectangle tempRec = {msPos.x - (int)(width/2), msPos.y - (int)(height/2), width, height};
			DrawRectangleRec(tempRec, RED);
			
			/* text position and drawing */
			Vector2 strPos = {tempRec.x+2, tempRec.y+2};
			DrawTextEx(unifont, buttons[saveInst].str, strPos, 16, 1, WHITE);
		}
		
		
		/* check if the mouse button has been released after it has been
		   pressed, and if it was over a collision area */
		struct list *tmp;
		for(tmp = list; tmp != NULL; tmp = tmp->next)
		{
			if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmp->draw.collisionRec) && btnEnable == 1)
			{
				if(whilecond == 1)
				{
					tmp = insertNext(tmp, whiletail, unifont);
					tmp = insertNext(tmp, whilehead, unifont);
					whilecond = 0;
				} else if(ifcond == 1)
				{
					tmp = insertNext(tmp, iftail, unifont);
					tmp = insertNext(tmp, ifhead, unifont);
					ifcond = 0;
				} else
				{
					tmp = insertNext(tmp, stmt, unifont);
				}
				list = setOffset(list);
				btnEnable = 0;
			}
		}
		
		/**********************************
		* dragging code below, do not touch
		**********************************/
		
		/* if one of the instructions is dragged */
		for(tmp = list; tmp != NULL; tmp = tmp->next)
		{
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmp->draw.rec) && tmp->draw.stmts != whiletail && tmp->draw.stmts != iftail && movEnable == 0)
			{
				save = tmp;
				if(tmp->draw.stmts == whilehead || tmp->draw.stmts == ifhead)
				{
					int tempLength = 1;
					
					struct list *l;
					int n = 1;
					for(l = tmp->next; n > 0; l = l->next)
					{
						if(l->draw.stmts == whilehead || l->draw.stmts == ifhead) n++;
						if(l->draw.stmts == whiletail || l->draw.stmts == iftail) n--;
						
						tempLength++;
					}
					
					blockLength = tempLength;
				}
				
				if(tmp->draw.stmts == whilehead || tmp->draw.stmts == ifhead)
				{	
					tmp->draw.clr = GREEN;
					tmp->draw.strClr = GREEN;	
			
					struct list *color;
					int n = 1;
					for(color = tmp->next; n > 0; color = color->next)
					{
						if(color->draw.stmts == whilehead || color->draw.stmts == ifhead) n++;
						if(color->draw.stmts == whiletail || color->draw.stmts == iftail) n--;
						
						color->draw.clr = GREEN;
						color->draw.strClr = GREEN;	
					}
				} else
				{
					tmp->draw.clr = GREEN;
					tmp->draw.strClr = GREEN;
				}
				movEnable = 1;
			} else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmp->draw.collisionRec) && movEnable == 1)
			{
				int unable = 0;
				if(save->draw.stmts == whilehead || save->draw.stmts == ifhead)
				{
					struct list *test;
					int i = 0;
					for(test = save; i<blockLength; i++)
					{
						if(test == tmp) unable = 1;
						test = test->next;
					}
				}
				
				if(tmp->next != save && tmp->next != save->next && unable == 0)
				{
					if(save->draw.stmts == whilehead || save->draw.stmts == ifhead)
					{
						struct list *insert;
						for(insert = list; insert->next != save; insert = insert->next);
						
						int i;
						for(i = 0; i < blockLength; i++)
						{
							tmp = insertNext(tmp, insert->next->draw.stmts, unifont);
							tmp = tmp->next;
							insert = insert->next;
						}
					} else
					{
						struct list *del;
						for(del = list; del->next != save; del = del->next);
						del = deleteNext(del);
						
						tmp = insertNext(tmp, save->draw.stmts, unifont);
					}
					
					struct list *color;
					for(color = list; color->next != NULL; )
					{
						if(color->next->draw.clr.r == 0   &&
						   color->next->draw.clr.g == 228 &&
						   color->next->draw.clr.b == 48  &&
						   color->next->draw.clr.a == 255)
						{
							color = deleteNext(color);
						} else
						{
							color = color->next;
						}
					}
				}
				
				movEnable = 0;
				
				list = setOffset(list);
			}
		}
		
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			movEnable = 0;
			btnEnable = 0;
		}
		
		if(movEnable == 1)
		{
			hover(msPos, list);
		} else
		{
			struct list *color;
			for(color = list->next; color != NULL; color = color->next)
			{
				color->draw.clr = BLUE;
				color->draw.strClr = RAYWHITE;
			}
		}
		
		/**********************************
		* dragging code above, do not touch
		**********************************/
		
		ClearBackground(RAYWHITE);
		EndDrawing();
	}
	
	return 0;
}