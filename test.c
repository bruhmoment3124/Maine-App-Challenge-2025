#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "stuff.h"

enum stmts
{
	start_st,
	add_st,
	sub_st,
	mult_st,
	move_st,
	turn_st,
	set_st,
	let_st,
	whilehead_st,
	whiletail_st,
	ifhead_st,
	iftail_st
};

enum instType_gr
{
	reg_gr,
	var_gr,
	cond_gr,
	dir_gr
};

struct inst_gr
{
	enum instType_gr type;
	char *text;
	Rectangle rec;
	Color clr;
	struct inst_gr *next;
};

struct draw
{
	enum stmts stmts;
	struct inst_gr *inst;
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

struct inst_gr * createInst(struct inst_gr *inst, enum instType_gr type, char *text, Font fnt)
{
	inst = realloc(inst, sizeof(struct inst_gr));
	inst->type = type;
	
	inst->text = calloc(strlen(text)+1, 1);
	strcat(inst->text, text);
	
	Vector2 size = MeasureTextEx(fnt, inst->text, 16, 1);
	inst->rec.width = size.x+4;
	inst->rec.height = size.y+4;
	
	inst->next = NULL;
	
	return inst;
}

/* change one of the parts of an instruction */
struct inst_gr * changeInst(struct inst_gr *inst, char *text, Font fnt)
{
	inst->text = realloc(inst->text, strlen(text)+1);
	memset(inst->text, 0, strlen(text)+1);
	strcat(inst->text, text);
	
	Vector2 size = MeasureTextEx(fnt, inst->text, 16, 1);
	inst->rec.width = size.x+4;
	inst->rec.height = size.y+4;
	
	return inst;
}

struct inst_gr * setOffsetX(struct inst_gr *inst, int xoffset, int yoffset)
{
	int tmpXoffset = xoffset;
	int tmpYoffset = yoffset;
	
	struct inst_gr *tmp;
	for(tmp = inst; tmp != NULL; tmp = tmp->next)
	{
		tmp->rec.x = tmpXoffset;
		tmp->rec.y = tmpYoffset;
		tmpXoffset += tmp->rec.width;
	}
	
	return inst;
}

struct list * setGreen(struct list *list)
{
	struct inst_gr *tmp;
	for(tmp = list->draw.inst; tmp != NULL; tmp = tmp->next) tmp->clr = GREEN;
	
	return list;
}

struct list * setBack(struct list *list)
{
	struct inst_gr *tmp;
	for(tmp = list->draw.inst; tmp != NULL; tmp = tmp->next)
	{
		switch(tmp->type)
		{
			case reg_gr:
				tmp->clr = BLUE;
			break;
			
			case var_gr:
				tmp->clr = ORANGE;
			break;
			
			case cond_gr:
				tmp->clr = PURPLE;
			break;
			
			case dir_gr:	
				tmp->clr = RED;
			break;
		}
	}
	
	return list;
}


void drawInst(struct inst_gr *inst, Font fnt)
{
	struct inst_gr *tmp;
	for(tmp = inst; tmp != NULL; tmp = tmp->next)
	{
		Vector2 pos = {tmp->rec.x+2, tmp->rec.y+2};
		DrawRectangleRec(tmp->rec, tmp->clr);
		DrawTextEx(fnt, tmp->text, pos, 16, 1, WHITE);
	}
}

struct list * insertNext(struct list *list, enum stmts stmt, Font fnt, char *optVar1, char *optVar2, char *optCond, char *optDir)
{
	struct draw tempDraw;
	tempDraw.stmts = stmt;

	if(optVar1 == NULL) optVar1 = "   ";
	if(optVar2 == NULL) optVar2 = "   ";
	if(optCond == NULL) optCond = "   ";
	if(optDir == NULL) optDir = "   ";

	switch(stmt)
	{
		case add_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "add", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, reg_gr, "to", fnt);
			tempDraw.inst->next->next->clr = BLUE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
		break;
		
		case sub_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "subtract", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, reg_gr, "from", fnt);
			tempDraw.inst->next->next->clr = BLUE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
		break;
		
		case mult_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "multiply", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, reg_gr, "by", fnt);
			tempDraw.inst->next->next->clr = BLUE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
		break;
		
		case move_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "move turtle", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, dir_gr, optDir, fnt);
			tempDraw.inst->next->clr = RED;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->next->clr = ORANGE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, reg_gr, "px", fnt);
			tempDraw.inst->next->next->next->clr = BLUE;
		break;
		
		case turn_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "turn turtle", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, dir_gr, optDir, fnt);
			tempDraw.inst->next->clr = RED;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->next->clr = ORANGE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, reg_gr, "degrees", fnt);
			tempDraw.inst->next->next->next->clr = BLUE;
		break;
		
		case set_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "set", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, reg_gr, "equal to", fnt);
			tempDraw.inst->next->next->clr = BLUE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
		break;
		
		case let_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "let", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, reg_gr, "equal", fnt);
			tempDraw.inst->next->next->clr = BLUE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
		break;
		
		case whilehead_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "while", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, cond_gr, optCond, fnt);
			tempDraw.inst->next->next->clr = PURPLE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
			tempDraw.inst->next->next->next->next = createInst(tempDraw.inst->next->next->next->next, reg_gr, "do", fnt);
			tempDraw.inst->next->next->next->next->clr = BLUE;
		break;
		
		case whiletail_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "end while", fnt);
			tempDraw.inst->clr = BLUE;
		break;
		
		case ifhead_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "if", fnt);
			tempDraw.inst->clr = BLUE;
			tempDraw.inst->next = createInst(tempDraw.inst->next, var_gr, optVar1, fnt);
			tempDraw.inst->next->clr = ORANGE;
			tempDraw.inst->next->next = createInst(tempDraw.inst->next->next, cond_gr, optCond, fnt);
			tempDraw.inst->next->next->clr = PURPLE;
			tempDraw.inst->next->next->next = createInst(tempDraw.inst->next->next->next, var_gr, optVar2, fnt);
			tempDraw.inst->next->next->next->clr = ORANGE;
			tempDraw.inst->next->next->next->next = createInst(tempDraw.inst->next->next->next->next, reg_gr, "then", fnt);
			tempDraw.inst->next->next->next->next->clr = BLUE;
		break;
		
		case iftail_st:
			tempDraw.inst = createInst(tempDraw.inst, reg_gr, "end if", fnt);
			tempDraw.inst->clr = BLUE;
		break;
	}
	
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

struct list * setOffset(struct list *list, int x, int y)
{
	int xoffset = x, yoffset = y;
	
	struct list *tmp;
	for(tmp = list->next; tmp != NULL; tmp = tmp->next)
	{
		if(tmp->draw.stmts == whiletail_st ||
		   tmp->draw.stmts == iftail_st)
		{
			xoffset -= 20;
		}
		
		tmp->draw.inst = setOffsetX(tmp->draw.inst, xoffset, yoffset);
		
		if(tmp->draw.stmts == whilehead_st ||
		   tmp->draw.stmts == ifhead_st)
		{
			xoffset += 20;
		}
		
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
	int i = 0;
	
	struct list *tmp;
	for(tmp = list->next; (tmp != NULL) && (i < 13); tmp = tmp->next)
	{
		i++;
		drawInst(tmp->draw.inst, fnt);
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
	
	int xoffset = 75, yoffset = 375;
	
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
	
	int xoffset = 175, yoffset = 383;
	
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
	
	int xoffset = 275, yoffset = 375;
	
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
	
	int xoffset = 275, yoffset = 450;
	
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

int collideInstPressed(Vector2 msPos, struct inst_gr *inst)
{
	struct inst_gr *tmp;
	for(tmp = inst; tmp != NULL; tmp = tmp->next)
	{
		Rectangle tempRec = {tmp->rec.x, tmp->rec.y, tmp->rec.width, tmp->rec.height};
		if(CheckCollisionPointRec(msPos, tempRec)) return 1;
	}
	
	return 0;
}

int collideInstRelease(Vector2 msPos, struct inst_gr *inst)
{
	struct inst_gr *tmp;
	for(tmp = inst; tmp != NULL; tmp = tmp->next)
	{
		Rectangle tempRec = {tmp->rec.x, tmp->rec.y+10, tmp->rec.width, tmp->rec.height};
		if(CheckCollisionPointRec(msPos, tempRec)) return 1;
	}
	
	return 0;
}

int drawUnderline(Vector2 msPos, struct inst_gr *inst)
{
	struct inst_gr *tmp;
	for(tmp = inst; tmp != NULL; tmp = tmp->next)
	{
		Rectangle tempRec = {tmp->rec.x, tmp->rec.y+10, tmp->rec.width, tmp->rec.height};
		if(CheckCollisionPointRec(msPos, tempRec)) return 1;
	}
	
	return 0;
}

void hover(Vector2 msPos, struct list *list)
{
	int drawn = 0;
	
	/* draw green line below instructions */
	struct list *tmp;
	for(tmp = list->next; (tmp != NULL) && (drawn < 12); tmp = tmp->next)
	{
		if(drawUnderline(msPos, tmp->draw.inst))
		{
			struct inst_gr *i;
			for(i = tmp->draw.inst; i != NULL; i = i->next)
			{
				DrawRectangle(i->rec.x, i->rec.y+18, i->rec.width, 2, GREEN);
			}
		}
		drawn++;
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
	}
}

int ColorIsEqual(Color clr1, Color clr2)
{
	if(clr1.r == clr2.r &&
	   clr1.g == clr2.g &&
	   clr1.b == clr2.b &&
	   clr1.a == clr2.a) return 1;
	   
	return 0;
}

char * transcribe(struct list *list)
{
	int size = 1;
	char *tmpStr = calloc(1, 1);
	
	struct list *tmp;
	for(tmp = list->next; tmp != NULL; tmp = tmp->next)
	{
		struct inst_gr *tmpInst;
		for(tmpInst = tmp->draw.inst; tmpInst != NULL; tmpInst = tmpInst->next)
		{
			size += strlen(tmpInst->text);
			if(tmpInst->type == reg_gr) size++;
			
			tmpStr = realloc(tmpStr, size);
			if(tmpInst->type == reg_gr) strcat(tmpStr, " ");
			strcat(tmpStr, tmpInst->text);
		}
	}
	
	return tmpStr;
}



int main(void)
{
	/* below is instructions and initialization */
	struct list *list = malloc(sizeof(struct list));
	list->draw.stmts = start_st;
	list->draw.str = NULL;
	list->next = NULL;
	list->draw.xoffset = 50;
	list->draw.yoffset = 50;
	list->draw.collisionRec.x = 50;
	list->draw.collisionRec.y = 50;
	list->draw.collisionRec.width = 100;
	list->draw.collisionRec.height = 10;
	
	struct btns *buttons = malloc(9 * sizeof(struct btns));
	struct vars *variables = malloc(9 * sizeof(struct vars));
	struct conds *conditionals = malloc(3 * sizeof(struct conds));
	struct dirs *directions = malloc(4 * sizeof(struct dirs));
	
	InitWindow(750, 600, "test");
	
	SetTargetFPS(60);
	
	Font unifont = LoadFont("unifont.otf");
	buttons = createButtons(buttons, unifont);
	variables = createVariables(variables, unifont);
	conditionals = createConditionals(conditionals, unifont);
	directions = createDirections(directions, unifont);
	
	enum stmts stmt;
	int whilecond = 0, ifcond = 0;
	
	int btnEnable = 0, movEnable = 0, varEnable = 0, condEnable = 0, dirEnable = 0, numEnable = 0;
	
	int saveInst, saveVar, saveCond, saveDir; /* save button instruction for displaying later */

	char *optVar[2], *optCond = NULL, *optDir = NULL;
	optVar[0] = NULL;
	optVar[1] = NULL;

	struct list *save = NULL;
	int blockLength = 0;
	
	int numVar = 0;
	
	/* above is instructions */
	
	/* general ui */
	Color startBtnClr = GREEN;
	int interp = 0;
	int currentInst = 0;
	
	int x = 50, y = 50;
	
	struct list *part = list;
	
	int emitSize = 0;
	Vector2 initPos = {600, 150};
	Vector2 *emit = NULL;
	emit = emitLine(emit, initPos, &emitSize);
	float rotation = 0.0;
	
	/* interpreter setup */
	
	struct inst *instList;
	int instListSize = 0;
	
	int state = 0;
	
	while(!WindowShouldClose())
	{
		BeginDrawing();
		
		if(state == 0)
		{
			char *code;
	
			struct token tk; /* get first token */
			
			/* symbol table stack init */
			struct stack st;
			st.tb = NULL;
			st.top = -1;
					
			/* table of saved pointers for deallocation after
			   values are popped off the stack */
			struct table **saveTb = NULL;
			int saveTableSize = 0;
			
			/* draw instruction box */
			DrawRectangle(50, 350, 400, 225, GRAY);
			DrawRectangleLines(50, 350, 400, 225, BLACK);
			
			
			/* draw code box */
			DrawRectangle(50, 50, 400, 260, GRAY);
			
			Rectangle trash = {395, 255, 50, 50};
			DrawRectangleRec(trash, RED);
			
			/* draw state buttons */
			
			Vector2 msPos = GetMousePosition();
			
			/* code button */
			Vector2 codeBtnSize = MeasureTextEx(unifont, "code", 16, 1);
			int codeWidth = codeBtnSize.x+5;
			int codeHeight = codeBtnSize.y+5;
			
			Rectangle codeBtn = {50, 10, codeWidth+100, codeHeight};
			DrawRectangleRounded(codeBtn, 100, 1000, RED);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, codeBtn)) state = 0;
			
			Vector2 codeStrPos = {codeBtn.x+52, codeBtn.y+2};
			DrawTextEx(unifont, "code", codeStrPos, 16, 1, WHITE);
			
			/* docs button */
			Vector2 docsBtnSize = MeasureTextEx(unifont, "docs", 16, 1);
			int docsWidth = docsBtnSize.x+5;
			int docsHeight = docsBtnSize.y+5;
			
			Rectangle docsBtn = {250, 10, docsWidth+100, docsHeight};
			DrawRectangleRounded(docsBtn, 100, 1000, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, docsBtn)) state = 1;
			
			Vector2 docsStrPos = {docsBtn.x+52, docsBtn.y+2};
			DrawTextEx(unifont, "docs", docsStrPos, 16, 1, WHITE);
			
			/* about button */
			Vector2 abtBtnSize = MeasureTextEx(unifont, "about", 16, 1);
			int abtWidth = abtBtnSize.x+5;
			int abtHeight = abtBtnSize.y+5;
			
			Rectangle abtBtn = {450, 10, abtWidth+100, abtHeight};
			DrawRectangleRounded(abtBtn, 100, 1000, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, abtBtn)) state = 2;
			
			Vector2 abtStrPos = {abtBtn.x+52, abtBtn.y+2};
			DrawTextEx(unifont, "about", abtStrPos, 16, 1, WHITE);
			   
			if(IsKeyPressed(KEY_UP) && part->next->next != NULL)
			{
				y -= 20;
				list = setOffset(list, x, y);
				part = part->next;
			}   
			
			if(IsKeyPressed(KEY_DOWN) && part != list)
			{
				y += 20;
				list = setOffset(list, x, y);
				
				struct list *tmp;
				for(tmp = list; tmp->next != part; tmp = tmp->next);
				part = tmp;
			}
			
			/* draw buttons */
			drawVals(part, unifont);
			drawButtons(buttons, unifont);
			drawVariables(variables, unifont);
			drawConditionals(conditionals, unifont);
			drawDirections(directions, unifont);
			
			/* check if a button is pressed */
			int i;
			for(i = 0; i<9; i++)
			{
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, buttons[i].box))
				{
					switch(buttons[i].type)
					{
						case 0: stmt = add_st; break;
						case 1: stmt = sub_st; break;
						case 2: stmt = mult_st; break;
						case 3: stmt = move_st; break;
						case 4: stmt = turn_st; break;
						case 5: stmt = set_st; break;
						case 6: stmt = let_st; break;
						case 7: whilecond = 1; break;
						case 8: ifcond = 1; break;
					}
					
					saveInst = i;
					btnEnable = 1;
				}
			}
			
			/* if a variable is pressed */
			int j;
			for(j = 0; j<9; j++)
			{
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(msPos, variables[j].pos, variables[j].rad))
				{
					saveVar = j;
					varEnable = 1;
				}
			}
			
			/* if a conditional is pressed */
			int k;
			for(k = 0; k<4; k++)
			{
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, conditionals[k].box))
				{
					saveCond = k;
					condEnable = 1;
				}
			}
			
			/* if a direction is pressed */
			int l;
			for(l = 0; l<4; l++)
			{
				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, directions[l].box))
				{
					saveDir = l;
					dirEnable = 1;
				}
			}
			
			/* if a button has been pressed, enable it */
			if(btnEnable == 1)
			{
				hover(msPos, part);
			
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
			
			if(varEnable == 1)
			{
				DrawCircle(msPos.x, msPos.y, 8, ORANGE);
				
				/* text position and drawing */
				Vector2 strPos = {msPos.x-4, msPos.y-9};
				DrawTextEx(unifont, variables[saveVar].str, strPos, 16, 1, WHITE);
			}
			
			if(condEnable == 1)
			{
				Vector2 tmpSize = MeasureTextEx(unifont, conditionals[saveCond].str, 16, 1);
				int width = tmpSize.x+5;
				int height = tmpSize.y+5;
				
				/* rectangle position and drawing */
				Rectangle tempRec = {msPos.x - (int)(width/2), msPos.y - (int)(height/2), width, height};
				DrawRectangleRec(tempRec, PURPLE);
				
				/* text position and drawing */
				Vector2 strPos = {tempRec.x+2, tempRec.y+2};
				DrawTextEx(unifont, conditionals[saveCond].str, strPos, 16, 1, WHITE);
			}
			
			if(dirEnable == 1)
			{
				Vector2 tmpSize = MeasureTextEx(unifont, directions[saveDir].str, 16, 1);
				int width = tmpSize.x+5;
				int height = tmpSize.y+5;
				
				/* rectangle position and drawing */
				Rectangle tempRec = {msPos.x - (int)(width/2), msPos.y - (int)(height/2), width, height};
				DrawRectangleRec(tempRec, RED);
				
				/* text position and drawing */
				Vector2 strPos = {tempRec.x+2, tempRec.y+2};
				DrawTextEx(unifont, directions[saveDir].str, strPos, 16, 1, WHITE);
			}
			
			char *numStr = TextFormat("%d", numVar);
			Vector2 size = MeasureTextEx(unifont, numStr, 16, 1);
			
			Rectangle tempRec1 = {225, 400, size.x+5, size.y+5};
			Vector2 strPos = {tempRec1.x+2, tempRec1.y+2};
			
			Rectangle tempRec2 = {225, 375, 20, 20};
			DrawRectangleRec(tempRec2, ORANGE);
			
			Rectangle tempRec3 = {225, 425, 20, 20};
			DrawRectangleRec(tempRec3, ORANGE);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tempRec1)) numEnable = 1;
			
			if(numEnable == 1)
			{
				/* rectangle position and drawing */
				Rectangle tempRec = {msPos.x - (int)(tempRec1.width/2), msPos.y - (int)(tempRec1.height/2), tempRec1.width, tempRec1.height};
				DrawRectangleRec(tempRec, ORANGE);
				
				/* text position and drawing */
				Vector2 strPos = {tempRec.x+2, tempRec.y+2};
				DrawTextEx(unifont, numStr, strPos, 16, 1, WHITE);
			}
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tempRec2)) numVar++;
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tempRec3)) numVar--;
			
			DrawRectangleRec(tempRec1, ORANGE);
			DrawTextEx(unifont, numStr, strPos, 16, 1, WHITE);
			
			/*************************************************************/
			/*-----------------------------------------------------------*/
			/*************************************************************/
			
			/* check if the mouse button has been released after it has been
			   pressed, and if it was over a collision area */
			struct list *tmp;
			for(tmp = list; tmp != NULL; tmp = tmp->next)
			{
				struct inst_gr *tmpInst;
				for(tmpInst = tmp->draw.inst; tmpInst != NULL; tmpInst = tmpInst->next)
				{
					if((IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && collideInstRelease(msPos, tmpInst) && btnEnable == 1) ||
					   (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, list->draw.collisionRec) && btnEnable == 1))
					{
						if(whilecond == 1)
						{
							tmp = insertNext(tmp, whiletail_st, unifont, NULL, NULL, NULL, NULL);
							tmp = insertNext(tmp, whilehead_st, unifont, NULL, NULL, NULL, NULL);
							whilecond = 0;
						} else if(ifcond == 1)
						{
							tmp = insertNext(tmp, iftail_st, unifont, NULL, NULL, NULL, NULL);
							tmp = insertNext(tmp, ifhead_st, unifont, NULL, NULL, NULL, NULL);
							ifcond = 0;
						} else
						{
							tmp = insertNext(tmp, stmt, unifont, NULL, NULL, NULL, NULL);
						}
						list = setOffset(list, x, y);
						btnEnable = 0;
					}
				}
			}
			
			/**********************************
			* dragging code below, do not touch
			**********************************/
			
			/* if one of the instructions is dragged */
			for(tmp = list; tmp != NULL; tmp = tmp->next)
			{
				struct inst_gr *tmpInst;
				for(tmpInst = tmp->draw.inst; tmpInst != NULL; tmpInst = tmpInst->next)
				{
					if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && collideInstPressed(msPos, tmpInst) && tmp->draw.stmts != whiletail_st && tmp->draw.stmts != iftail_st && movEnable == 0)
					{
						tmp = setGreen(tmp);
						save = tmp;
						if(tmp->draw.stmts == whilehead_st || tmp->draw.stmts == ifhead_st)
						{
							int tempLength = 1;
							
							struct list *l;
							int n = 1;
							for(l = tmp->next; n > 0; l = l->next)
							{
								if(l->draw.stmts == whilehead_st || l->draw.stmts == ifhead_st) n++;
								if(l->draw.stmts == whiletail_st || l->draw.stmts == iftail_st) n--;
								
								tempLength++;
							}
							
							blockLength = tempLength;
						}
						
						if(tmp->draw.stmts == whilehead_st || tmp->draw.stmts == ifhead_st)
						{	
							tmp = setGreen(tmp);
					
							struct list *color;
							int n = 1;
							for(color = tmp->next; n > 0; color = color->next)
							{
								if(color->draw.stmts == whilehead_st || color->draw.stmts == ifhead_st) n++;
								if(color->draw.stmts == whiletail_st || color->draw.stmts == iftail_st) n--;
							
								color = setGreen(color);
							}
						} else
						{
							tmp = setGreen(tmp);
						}
						movEnable = 1;
					} else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && collideInstRelease(msPos, tmpInst) && movEnable == 1)
					{
						int unable = 0;
						if(save->draw.stmts == whilehead_st || save->draw.stmts == ifhead_st)
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
							if(save->draw.stmts == whilehead_st || save->draw.stmts == ifhead_st)
							{
								struct list *insert;
								for(insert = list; insert->next != save; insert = insert->next);
								
								int i;
								for(i = 0; i < blockLength; i++)
								{
									optVar[0] = NULL;
									optVar[1] = NULL;
									optCond = NULL;
									optDir = NULL;
									
									int varNum = 0;
									struct inst_gr *tmpInst;
									for(tmpInst = insert->next->draw.inst; tmpInst != NULL; tmpInst = tmpInst->next)
									{
										if(tmpInst->type == var_gr)
										{
											optVar[varNum] = tmpInst->text;
											varNum++;
										}
										
										if(tmpInst->type == cond_gr) optCond = tmpInst->text;
										if(tmpInst->type == dir_gr) optDir = tmpInst->text;
									}
									
									tmp = insertNext(tmp, insert->next->draw.stmts, unifont, optVar[0], optVar[1], optCond, optDir);
									tmp = tmp->next;
									insert = insert->next;
								}
							} else
							{
								struct list *del;
								for(del = list; del->next != save; del = del->next);
								del = deleteNext(del);
								
								optVar[0] = NULL;
								optVar[1] = NULL;
								optCond = NULL;
								optDir = NULL;
								
								int varNum = 0;
								struct inst_gr *tmpInst;
								for(tmpInst = save->draw.inst; tmpInst != NULL; tmpInst = tmpInst->next)
								{
									if(tmpInst->type == var_gr)
									{
										optVar[varNum] = tmpInst->text;
										varNum++;
									}
									
									if(tmpInst->type == cond_gr) optCond = tmpInst->text;
									if(tmpInst->type == dir_gr) optDir = tmpInst->text;
								}
								
								tmp = insertNext(tmp, save->draw.stmts, unifont, optVar[0], optVar[1], optCond, optDir);
							}
							
							struct list *color;
							for(color = list; color->next != NULL; )
							{
								if(color->next->draw.inst->clr.r == 0   &&
								   color->next->draw.inst->clr.g == 228 &&
								   color->next->draw.inst->clr.b == 48  &&
								   color->next->draw.inst->clr.a == 255)
								{
									color = deleteNext(color);
								} else
								{
									color = color->next;
								}
							}
						}
						
						tmp = setBack(tmp);
						
						movEnable = 0;
						
						list = setOffset(list, x, y);
					}
					
					if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, trash) && movEnable == 1)
					{
						if(save->draw.stmts == whilehead_st || save->draw.stmts == ifhead_st)
						{
							struct list *del;
							for(del = list; del->next != save; del = del->next);
							
							int i;
							for(i = 0; i<blockLength; i++)
							{
								del = deleteNext(del);
								optVar[0] = NULL;
								optVar[1] = NULL;
								optCond = NULL;
								optDir = NULL;
							}
						} else
						{
							struct list *del;
							for(del = list; del->next != save; del = del->next);
							del = deleteNext(del);
							
							optVar[0] = NULL;
							optVar[1] = NULL;
							optCond = NULL;
							optDir = NULL;
						}
						
						movEnable = 0;
						
						list = setOffset(list, x, y);
					}
				}
			}
			
			/* don't touch above code */
			
			/* dragging variables, conditionals, and directions */
			for(tmp = list; tmp != NULL; tmp = tmp->next)
			{
				struct inst_gr *tmpInst;
				for(tmpInst = tmp->draw.inst; tmpInst != NULL; tmpInst = tmpInst->next)
				{
					if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmpInst->rec) && tmpInst->type == var_gr && varEnable == 1)
					{
						char *tempStr = calloc(strlen(variables[saveVar].str)+3, 1);
						strcat(tempStr, " ");
						strcat(tempStr, variables[saveVar].str);
						strcat(tempStr, " ");
						
						tmpInst = changeInst(tmpInst, tempStr, unifont);
						tmpInst = setOffsetX(tmpInst, tmpInst->rec.x, tmpInst->rec.y);
					}
					
					if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmpInst->rec) && tmpInst->type == cond_gr && condEnable == 1)
					{
						char *tempStr = calloc(strlen(conditionals[saveCond].str)+3, 1);
						strcat(tempStr, " ");
						strcat(tempStr, conditionals[saveCond].str);
						strcat(tempStr, " ");
						
						tmpInst = changeInst(tmpInst, tempStr, unifont);
						tmpInst = setOffsetX(tmpInst, tmpInst->rec.x, tmpInst->rec.y);
					}
					
					if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmpInst->rec) && tmpInst->type == dir_gr && dirEnable == 1)
					{
						char *tempStr = calloc(strlen(directions[saveDir].str)+3, 1);
						strcat(tempStr, " ");
						strcat(tempStr, directions[saveDir].str);
						strcat(tempStr, " ");
						
						tmpInst = changeInst(tmpInst, tempStr, unifont);
						tmpInst = setOffsetX(tmpInst, tmpInst->rec.x, tmpInst->rec.y);
					}
					
					if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, tmpInst->rec) && tmpInst->type == var_gr && numEnable == 1)
					{
						char *tempStr = calloc(strlen(numStr)+3, 1);
						strcat(tempStr, " ");
						strcat(tempStr, numStr);
						strcat(tempStr, " ");
						
						tmpInst = changeInst(tmpInst, tempStr, unifont);
						tmpInst = setOffsetX(tmpInst, tmpInst->rec.x, tmpInst->rec.y);
					}
				}
			}
			
			if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			{
				btnEnable = 0;
				movEnable = 0;
				varEnable = 0;
				condEnable = 0;
				dirEnable = 0;
				numEnable = 0;
			}
			
			if(movEnable == 1)
			{
				hover(msPos, part);
			} else
			{
				struct list *color;
				for(color = list->next; color != NULL; color = color->next) color = setBack(color);
			}
			
			/* instruction stuff above */
			
			Rectangle startBtnRec = {550, 285, 100, 25};
			DrawRectangleRec(startBtnRec, startBtnClr);
			
			Rectangle canvas = {500, 50, 200, 200};
			DrawRectangleLinesEx(canvas, 5.0, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, startBtnRec) && ColorIsEqual(startBtnClr, GREEN))
			{
				startBtnClr = RED;
				
				code = NULL;
				code = transcribe(list);
				
				tk = getNextToken(&code); /* get first token */
				
				instList = NULL;
				instListSize = 0;
				currentInst = 0;
				
				int label = 0;
				
				int halt = 0;
				
				int fail = parse(&code, &tk, &st, &saveTb, &saveTableSize, &instList, &instListSize, &label, &halt);
			
				if(fail)
				{
					interp = 0;
				} else
				{
					interp = 1;
				}
			} else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, startBtnRec) && ColorIsEqual(startBtnClr, RED))
			{
				startBtnClr = GREEN;
				interp = 0;
				
				emit = NULL;
				emitSize = 0;
				Vector2 initPos = {600, 150};
				emit = emitLine(emit, initPos, &emitSize);
				
				rotation = 0.0;
				
				instList = NULL;
				instListSize = 0;
				currentInst = 0;
			}
			
			if(interp == 1)
			{
				int done = interpret(instList, instListSize, &currentInst, &emit, &emitSize, &rotation);
				
				if(done)
				{
					currentInst = 0;
					interp = 0;
				}
			}
			
			int drawLines;
			for(drawLines = 0; drawLines<emitSize-1; drawLines++)
			{
				DrawLineV(emit[drawLines], emit[drawLines+1], GREEN);
			}
		} else if(state == 1)
		{
			Vector2 msPos = GetMousePosition();
			
			/* code button */
			Vector2 codeBtnSize = MeasureTextEx(unifont, "code", 16, 1);
			int codeWidth = codeBtnSize.x+5;
			int codeHeight = codeBtnSize.y+5;
			
			Rectangle codeBtn = {50, 10, codeWidth+100, codeHeight};
			DrawRectangleRounded(codeBtn, 100, 1000, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, codeBtn)) state = 0;
			
			Vector2 codeStrPos = {codeBtn.x+52, codeBtn.y+2};
			DrawTextEx(unifont, "code", codeStrPos, 16, 1, WHITE);
			
			/* docs button */
			Vector2 docsBtnSize = MeasureTextEx(unifont, "docs", 16, 1);
			int docsWidth = docsBtnSize.x+5;
			int docsHeight = docsBtnSize.y+5;
			
			Rectangle docsBtn = {250, 10, docsWidth+100, docsHeight};
			DrawRectangleRounded(docsBtn, 100, 1000, RED);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, docsBtn)) state = 1;
			
			Vector2 docsStrPos = {docsBtn.x+52, docsBtn.y+2};
			DrawTextEx(unifont, "docs", docsStrPos, 16, 1, WHITE);
			
			/* about button */
			Vector2 abtBtnSize = MeasureTextEx(unifont, "about", 16, 1);
			int abtWidth = abtBtnSize.x+5;
			int abtHeight = abtBtnSize.y+5;
			
			Rectangle abtBtn = {450, 10, abtWidth+100, abtHeight};
			DrawRectangleRounded(abtBtn, 100, 1000, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, abtBtn)) state = 2;
			
			Vector2 abtStrPos = {abtBtn.x+52, abtBtn.y+2};
			DrawTextEx(unifont, "about", abtStrPos, 16, 1, WHITE);
		} else if(state == 2)
		{
			Vector2 msPos = GetMousePosition();
			
			/* code button */
			Vector2 codeBtnSize = MeasureTextEx(unifont, "code", 16, 1);
			int codeWidth = codeBtnSize.x+5;
			int codeHeight = codeBtnSize.y+5;
			
			Rectangle codeBtn = {50, 10, codeWidth+100, codeHeight};
			DrawRectangleRounded(codeBtn, 100, 1000, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, codeBtn)) state = 0;
			
			Vector2 codeStrPos = {codeBtn.x+52, codeBtn.y+2};
			DrawTextEx(unifont, "code", codeStrPos, 16, 1, WHITE);
			
			/* docs button */
			Vector2 docsBtnSize = MeasureTextEx(unifont, "docs", 16, 1);
			int docsWidth = docsBtnSize.x+5;
			int docsHeight = docsBtnSize.y+5;
			
			Rectangle docsBtn = {250, 10, docsWidth+100, docsHeight};
			DrawRectangleRounded(docsBtn, 100, 1000, BLACK);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, docsBtn)) state = 1;
			
			Vector2 docsStrPos = {docsBtn.x+52, docsBtn.y+2};
			DrawTextEx(unifont, "docs", docsStrPos, 16, 1, WHITE);
			
			/* about button */
			Vector2 abtBtnSize = MeasureTextEx(unifont, "about", 16, 1);
			int abtWidth = abtBtnSize.x+5;
			int abtHeight = abtBtnSize.y+5;
			
			Rectangle abtBtn = {450, 10, abtWidth+100, abtHeight};
			DrawRectangleRounded(abtBtn, 100, 1000, RED);
			
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(msPos, abtBtn)) state = 2;
			
			Vector2 abtStrPos = {abtBtn.x+52, abtBtn.y+2};
			DrawTextEx(unifont, "about", abtStrPos, 16, 1, WHITE);
		}
		
		ClearBackground(RAYWHITE);
		EndDrawing();
	}
	
	return 0;
}