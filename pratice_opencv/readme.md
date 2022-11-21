
## https://blog.csdn.net/youcans/article/details/124565960

### 处理文档。


过程：

（1）读取模板图像，将其转为灰度图，并且二值化；

（2）检测出模板图像中的数字轮廓；

（3）将模板中的数字轮廓按照 x 方向的坐标大小进行排序，匹配对应的数字，并把轮廓定义为统一大小；

（4）读取银行卡图像，转为灰度图像；

（5）定义卷积核，通过顶帽操作——原图像与开操作之间的差值图像，突出明亮的区域；

（6）用sobel算子处理图像；

（7）为提取出对应数字那几块区域，可以通过闭操作，连接数字，并且将图像二值化；

（8）目标区域内部未填充完全，再通过闭操作连通区域；

（9）遍历轮廓，根据目标轮廓的特征，筛选出符合的轮廓；

（10）遍历筛选出的轮廓，识别其中各内置轮廓，并且二值化，将轮廓大小定义为统一大小；

（11）和模板中的数字进行模板匹配，选出最相近的数字图像；

```
import cv2
import numpy as np
import argparse
import imutils
from imutils import contours


def ShowImage(name, image):
cv2.imshow(name, image)
cv2.waitKey(0)  # 等待时间，0表示任意键退出
cv2.destroyAllWindows()

img = cv2.imread('img.png')     #   读取模板图像
ShowImage('template', img)

image_Gray = cv2.cvtColor(img,   cv2.COLOR_RGB2GRAY)   # 将图像转化为 灰度图
ShowImage('image_Gray', image_Gray)

image_Binary = cv2.threshold(image_Gray, 177, 255, cv2.THRESH_BINARY_INV)[1]   # 转换为二值化图像,[1]表示返回二值化图像，[0]表示返回阈值177
ShowImage('image_Binary', image_Binary)

# 提取轮廓
refcnts, his = cv2.findContours(image_Binary.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
cv2.drawContours(img, refcnts, -1, (0,0,255), 2)
ShowImage('img', img)

# refcnts = sortContours(refcnts, method="LefttoRight")[0]
refcnts = contours.sort_contours(refcnts, method="left-to-right")[0]
digits= {}

# 遍历每个轮廓
for (i, c) in enumerate(refcnts):  # enumerate函数用于遍历序列中的元素以及它们的下标
(x, y, w, h) = cv2.boundingRect(c)
roi = image_Binary[y:y+h, x:x+w]
roi = cv2.resize(roi, (57, 88))

    digits[i] = roi

# 初始化卷积核
rectKernel = cv2.getStructuringElement(cv2.MORPH_RECT, (9,3))
sqKernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5,5))


# 读取图像，进行预处理
image = cv2.imread("card.jpg")
ShowImage('image', image)
image = imutils.resize(image, width=300)
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)      # 将图像转化为灰度图
ShowImage('gray', gray)

# 通过顶帽操作，突出更明亮的区域
tophat = cv2.morphologyEx(gray, cv2.MORPH_TOPHAT, rectKernel)
ShowImage('tophat', tophat)

gradx = cv2.Sobel(tophat, ddepth=cv2.CV_32F, dx=1, dy=0, ksize=-1)
grady = cv2.Sobel(tophat, ddepth=cv2.CV_32F, dx=0, dy=1, ksize=-1)
gradx = np.absolute(gradx)
minVal = np.min(gradx)
maxVal = np.max(gradx)
# (minVal, maxVal) = (np.min(gradx), np.max(gradx))
gradx = (255 * ((gradx - minVal) / (maxVal - minVal)))     # 保证值的范围在0-255之间
gradx = gradx.astype("uint8")

print(np.array(gradx).shape)
ShowImage('gradx', gradx)

# 通过闭操作，先膨胀后腐蚀，将数字连接在一块
gradx = cv2.morphologyEx(gradx, cv2.MORPH_CLOSE,rectKernel)
ShowImage('gradx', gradx)

# THRESH_OTSU会自动寻找合适的阈值，适合双峰，需要把阈值设置为0
thresh = cv2.threshold(gradx, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]

ShowImage('thresh', thresh)

# 再来一个闭合操作，填充白框内的黑色区域
thresh = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, sqKernel)
ShowImage('thresh2', thresh)

# 计算轮廓
threshCnts, his = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
cnts = threshCnts
cur_img = image.copy()
cv2.drawContours(cur_img, cnts, -1, (0,0,255), 2)
ShowImage('image', cur_img)

locs = []

# 遍历轮廓
for (i, c) in enumerate(cnts):   # 函数用于遍历序列中的元素以及它们的下标
# 计算矩形
(x, y, w, h) = cv2.boundingRect(c)
ar = w/float(h)
# 选择合适的区域，根据实际任务来，这里是四个数字为一组
if ar > 2.5 and ar < 5.0:
if (w > 40 and w < 85) and (h > 10 and h < 20):
# 把符合的留下
locs.append((x,y,w,h))

# 将符合的轮廓根据x的值，从左到右排序
locs = sorted(locs, key=lambda  x:x[0])
output =[]

# 遍历轮廓中的每一个数字
for (i,(gx, gy, gw, gh)) in  enumerate(locs):
groupOutput = []  #初始化链表
# 根据坐标提取每一个组，往外多取一点，要不然看不清楚
group = gray[gy-5:gy+gh+5,gx-5:gx+gw+5]
ShowImage('group', group)
# 预处理
group = cv2.threshold(group, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]  # 二值化
ShowImage('group', group)

    # 找到每一组的轮廓
    digitCnts, his = cv2.findContours(group.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#  digitCnts = sortContours(digitCnts, method="LefttoRight")[0]
    digitCnts = contours.sort_contours(digitCnts, method="left-to-right")[0] # 对找到的轮廓进行排序

    # 计算每一组中的每一个数值  
    for c in digitCnts:
        # 找到当前数值的轮廓，resize成合适的大小
        (x,y,w,h) = cv2.boundingRect(c)
        roi = group[y:y+h, x:x+w]
        roi = cv2.resize(roi, (57,88))
        ShowImage('roi', roi)
        scores = []
        for(digit, digitROI) in digits.items():
            # 模板匹配
            #
            result = cv2.matchTemplate(roi, digitROI, cv2.TM_CCOEFF)
            (_, score, _, _) = cv2.minMaxLoc(result)
            scores.append(score)
        # 得到最合适的数字
        groupOutput.append(str(np.argmax(scores)))
        # 画矩形和字体
        cv2.rectangle(image, (gx - 5, gy - 5), (gx+gw+5, gy+gh+5), (0,0,255),1)
        cv2.putText(image, "".join(groupOutput), (gx, gy-15), cv2.FONT_HERSHEY_SIMPLEX,0.65, (0,0,255),2)
        # 得到结果
        output.extend(groupOutput)

ShowImage('image', image)

```