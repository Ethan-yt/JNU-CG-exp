//
// Created by Ethan on 2017/11/16.
//

#include <queue>
#include <App.h>
#include "PolygonScan2D.h"

//定义用于边表ET和活动边表AET的通用类Edge
class Edge {
public:
    int ymax;
    float x;
    float dx;
    Edge *next;
};


void PolygonScan2D::drawMe() {

    // 计算最高点的y坐标
    int maxY = 0, minY = 1000000;
    for (auto &point : points) {
        if (point.y > maxY) {
            maxY = point.y;
        }
        if (point.y < minY) {
            minY = point.y;
        }
    }
    // 初始化ET和AET

    int h = maxY - minY;

    Edge **ET = new Edge *[h + 1];

    for (int i = 0; i < h + 1; i++) {
        ET[i] = new Edge();
        ET[i]->next = nullptr;
    }
    //活动边表
    Edge *AET = new Edge();
    AET->next = nullptr;

    glBegin(GL_POINTS);

    //建立边表ET
    for (int i = 0; i < points.size(); i++) {
        //取出当前点1前后相邻的共4个点，点1与点2的连线作为本次循环处理的边，另外两个点点0和点3用于计算奇点
        Point2D p0 = points[(i - 1 + points.size()) % points.size()];
        Point2D p1 = points[i];
        Point2D p2 = points[(i + 1) % points.size()];
        Point2D p3 = points[(i + 2) % points.size()];

        int x0 = (int) p0.x, x1 = (int) p1.x, x2 = (int) p2.x, x3 = (int) p3.x;
        int y0 = (int) p0.y, y1 = (int) p1.y, y2 = (int) p2.y, y3 = (int) p3.y;

        y0 += -minY;
        y1 += -minY;
        y2 += -minY;
        y3 += -minY;

        //水平线直接舍弃
        if (p1.y == p2.y)
            continue;
        //分别计算下端点y坐标、上端点y坐标、下端点x坐标和斜率倒数
        int ymin = y1 > y2 ? y2 : y1;
        int ymax = y1 > y2 ? y1 : y2;
        float x = y1 > y2 ? x2 : x1;  // y值最小的端点的x坐标
        float dx = (float) (x1 - x2) / (y1 - y2); //斜率1/m
        //奇点特殊处理，若点2->1->0的y坐标单调递减则y1为奇点，若点1->2->3的y坐标单调递减则y2为奇点
        if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3))) {
            ymin++;
            x += dx;
        }
        //创建新边，插入边表ET
        Edge *p = new Edge();
        p->ymax = ymax;
        p->x = x;
        p->dx = dx;
        p->next = ET[ymin]->next;
        ET[ymin]->next = p;
    }

    //扫描线从下往上扫描，y坐标每次加1
    for (int i = 0; i < h; i++) {
        //取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
        while (ET[i]->next) {
            //取出ET中当前扫描行表头位置的边
            Edge *pInsert = ET[i]->next;
            Edge *p = AET;
            //在AET中搜索合适的插入位置
            while (p->next) {
                if (pInsert->x > p->next->x) {
                    p = p->next;
                    continue;
                }
                if (pInsert->x == p->next->x && pInsert->dx > p->next->dx) {
                    p = p->next;
                    continue;
                }
                //找到位置
                break;
            }
            //将pInsert从ET中删除，并插入AET的当前位置
            ET[i]->next = pInsert->next;
            pInsert->next = p->next;
            p->next = pInsert;
        }

        //AET中的边两两配对并填色
        Edge *p = AET;
        while (p->next && p->next->next) {
            for (int x = p->next->x; x < p->next->next->x; x++) {
                glVertex2i(x, i + minY);
            }
            p = p->next->next;
        }

        //删除AET中满足y=ymax的边
        p = AET;
        while (p->next) {
            if (p->next->ymax == i) {
                Edge *pDelete = p->next;
                p->next = pDelete->next;
                pDelete->next = nullptr;
                delete pDelete;
            } else {
                p = p->next;
            }
        }

        //更新AET中边的x值，进入下一循环
        p = AET;
        while (p->next) {
            p->next->x += p->next->dx;
            p = p->next;
        }

    }
    delete AET;
    delete ET;
    glEnd();
}
