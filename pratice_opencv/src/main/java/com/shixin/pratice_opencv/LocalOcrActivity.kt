package com.shixin.pratice_opencv

import android.annotation.SuppressLint
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.os.Environment
import android.os.Handler
import android.os.Message
import android.text.TextUtils
import androidx.appcompat.app.AppCompatActivity
import androidx.databinding.DataBindingUtil
import com.blankj.utilcode.util.FileIOUtils
import com.googlecode.tesseract.android.TessBaseAPI
import com.shixin.pratice_opencv.databinding.ActivityLocalocrBinding
import org.opencv.android.BaseLoaderCallback
import org.opencv.android.LoaderCallbackInterface
import org.opencv.android.OpenCVLoader
import org.opencv.android.Utils
import org.opencv.core.*
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.*
import java.io.File
import kotlin.concurrent.thread
import org.opencv.imgproc.Imgproc.THRESH_OTSU

import org.opencv.imgproc.Imgproc.THRESH_BINARY

import org.opencv.imgproc.Imgproc.threshold





class LocalOcrActivity : AppCompatActivity() {
    private lateinit var binding: ActivityLocalocrBinding
    private var ocrDataFilePath = "" //数据识别的文件路径

    @SuppressLint("HandlerLeak")
    private val handler = object : Handler() {
        override fun handleMessage(msg: Message) {
            when (msg.what) {
                0 -> {
                    val info: String = msg.obj as String
                    binding.activityLocalocrTvOcrResult.text = info
                }
            }

            super.handleMessage(msg)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = DataBindingUtil.setContentView<ActivityLocalocrBinding>(
            this,
            R.layout.activity_localocr
        )
        binding.lifecycleOwner = this
    }

    override fun onResume() {
        super.onResume()
        if (!OpenCVLoader.initDebug()) {
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback)
        } else {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS)
        }
    }

    private val mLoaderCallback: BaseLoaderCallback = object : BaseLoaderCallback(this) {
        override fun onManagerConnected(status: Int) {
            when (status) {
                SUCCESS -> {
                    //bankOcr()
                    thread {
                        findQrCode()
                    }
                }
                else -> {
                    super.onManagerConnected(status)
                }
            }
        }
    }

    private fun bankOcr() {
        //导入源图像
        val bitmap: Bitmap =
            BitmapFactory.decodeResource(resources, R.drawable.ic_bankcard)

        val src = Mat()
        Utils.bitmapToMat(bitmap, src) //将bitmap转换为Mat
        val thresholdImage = Mat(src.size(), src.type()) //这个二值图像用于找出关键信息的图像
        val cannyImage = Mat(src.size(), src.type())
        //将图像转换为灰度图像
        Imgproc.cvtColor(src, thresholdImage, Imgproc.COLOR_RGBA2GRAY)
        //将图像转换为边缘二值图像
        Imgproc.threshold(
            thresholdImage,
            thresholdImage,
            60.0,
            255.0,
            Imgproc.THRESH_BINARY
        )

        //开操作让信息联结到一起方便查出数字的位置
        var kernel = Imgproc.getStructuringElement(
            Imgproc.MORPH_RECT,
            Size(bitmap.width * 0.08, bitmap.width * 0.036)
        ) //获取结构元素
        Imgproc.morphologyEx(thresholdImage, cannyImage, Imgproc.MORPH_OPEN, kernel)

        //查找边缘
        //apertureSize---Sobel算子的大小，
        //
        //L2gradient---是否使用L2范数来计算图像梯度幅值。
        Imgproc.Canny(
            cannyImage, cannyImage, 100.0,
            200.0, 3
        )
        //膨胀让边缘更明显
        kernel = Imgproc.getStructuringElement(
            Imgproc.MORPH_RECT,
            Size(bitmap.width * 0.0036, bitmap.width * 0.0036)
        ) //获取结构元素
        Imgproc.dilate(cannyImage, cannyImage, kernel)

        //轮廓发现
        val hierarchy = Mat()
        val contours: ArrayList<MatOfPoint> = ArrayList()
        Imgproc.findContours(
            cannyImage,
            contours,
            hierarchy,
            Imgproc.RETR_EXTERNAL,
            Imgproc.CHAIN_APPROX_NONE
        )
        //找出信息所在的轮廓
        val allRect = ArrayList<Rect>()
        contours.forEach {
            val rect = Imgproc.boundingRect(it)
            //计算出数字所在位置的高与宽的大致比例
            val widthHeightRatio = rect.height.toDouble() / rect.width.toDouble()
            //剔除一部分明显不是信息位置的
            if (rect.x != 0 && rect.x + rect.width != src.width() && widthHeightRatio >= 0.05 && widthHeightRatio <= 0.09) {
                //对区域进行小量的扩充，方便识别数据
                rect.x = rect.x - 10
                rect.y = rect.y - 10
                rect.width = rect.width + 20
                rect.height = rect.height + 20
                allRect.add(rect)
            }
        }
        //对外接矩形的面积进行排序
        allRect.sortByDescending { it.width * it.height }
        //提取面积最大的矩形区域
        val infoRectData = allRect.take(1)
        //画出信息所在的位置
        val showInfoRectImg = Mat()
        src.copyTo(showInfoRectImg)
        infoRectData.forEach {
            Imgproc.rectangle(
                showInfoRectImg, it,
                Scalar(0.0, 255.0, 0.0, 255.0), (bitmap.width * 0.003).toInt(), 8
            )
        }


        val binaryBitmap =
            Bitmap.createBitmap(bitmap.width, bitmap.height, bitmap.config)
        Utils.matToBitmap(thresholdImage, binaryBitmap)
        ocrInfo(binaryBitmap, infoRectData)

        //释放资源
        thresholdImage.release()
        cannyImage.release()
        src.release()
        showInfoRectImg.release()
    }

    /**
     * 根据信息所在的位置，识别信息
     */
    private fun ocrInfo(dstBitmap: Bitmap, infoRect: List<Rect>) {
        thread {
            initOcrData()
            if (!TextUtils.isEmpty(ocrDataFilePath)) {
                // 开始调用Tess函数对图像进行识别
                val tessBaseAPI = TessBaseAPI()
                tessBaseAPI.setDebug(true)
                tessBaseAPI.init(ocrDataFilePath, "enm")
                tessBaseAPI.setVariable(TessBaseAPI.VAR_CHAR_WHITELIST, "0123456789") // 识别白名单
                tessBaseAPI.setVariable(
                    TessBaseAPI.VAR_CHAR_BLACKLIST,
                    "ABCDEFGHIJKLMNOPQRSTUVWYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+=-[]}{;:'\"\\|~`,./<>?…】丨"
                ) // 识别黑名单
                tessBaseAPI.pageSegMode = TessBaseAPI.PageSegMode.PSM_AUTO//设置识别模式

                infoRect.forEach {
                    val ocrBitmap = Bitmap.createBitmap(dstBitmap, it.x, it.y, it.width, it.height)
                    tessBaseAPI.setImage(ocrBitmap)//设置需要识别图片的bitmap
                    val number = tessBaseAPI.utF8Text
                    val msg = Message()
                    msg.what = 0
                    msg.obj = number
                    handler.sendMessage(msg)
                }
                tessBaseAPI.end()
            }
        }
    }

    /**
     * 加载数据识别的文件
     */
    private fun initOcrData() {
        val ocrDataStream = resources.openRawResource(R.raw.enm)
        getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS)?.let {
            ocrDataFilePath = it.absolutePath
            val ocrDataFile =
                File("${ocrDataFilePath}${File.separator}tessdata${File.separator}enm.traineddata")
            if (!ocrDataFile.exists()) {
                FileIOUtils.writeFileFromIS(ocrDataFile, ocrDataStream)
            }
        }
    }


    private fun findQrCode() {
        //导入源图像
        val bitmap: Bitmap =
            BitmapFactory.decodeResource(resources, R.drawable.icon_qrcode)


        val image = Mat()
        Utils.bitmapToMat(bitmap, image) //将bitmap转换为Mat
        //建立灰度图像存储空间
        val gray = Mat()
        //彩色图像灰度化
        cvtColor(image, gray, COLOR_RGB2GRAY)

        //高斯模糊
        val gauss = Mat()
        GaussianBlur(gray, gauss, Size(Point(15.0, 15.0)), 0.0)


        // 函数检测边缘
        val canny = Mat()
        Canny(gauss, canny, 50.0, 100.0)


        /*val binary = Mat()
        threshold(gray, binary, 0.0, 255.0, THRESH_BINARY or THRESH_OTSU)*/


        val imgDil = Mat()
        val kernel: Mat = getStructuringElement(MORPH_RECT, Size(3.0, 3.0))
        dilate(canny, imgDil, kernel)

        //找到轮廓
        val hierarchy = Mat()
        val contours: List<MatOfPoint> = ArrayList()
        findContours(
            imgDil,
            contours,
            hierarchy,
            RETR_TREE,
            CHAIN_APPROX_SIMPLE
        )

        val ints: MutableList<Int> = ArrayList()
        val points: MutableList<MatOfPoint> = ArrayList()

        println("共有" + contours.size + "个标志轮廓!")
        println("hierarchy cols = " + hierarchy.cols())
        println("hierarchy rows = " + hierarchy.rows())

        val binaryBitmap =
            Bitmap.createBitmap(bitmap.width, bitmap.height, bitmap.config)

        Utils.matToBitmap(imgDil, binaryBitmap)

        runOnUiThread {
            binding.img.setImageBitmap(binaryBitmap)
        }


        //从轮廓的拓扑结构信息中得到有5层以上嵌套的轮廓
        for (i in contours.indices) {
            val area: Double = contourArea(contours[i])
            // 2.1 初步过滤面积 7*7 = 49
            /*if (area < 49) {
                continue
            }*/
            //println("大于49")

            //drawContours(image, contours, i, Scalar(0.0, 0.0, 255.0), 4);

            var k = i
            var c = 0
            while (hierarchy.get(0, k)[2] != -1.0) {
                k = hierarchy.get(0, k)[2].toInt()
                println("hierarchy0====" + hierarchy.get(0, k)[0])
                println("hierarchy1====" + hierarchy.get(0, k)[1])
                c = c + 1
                println("c====$c")
                if (c >= 5) {
                    ints.add(i)
                    points.add(contours[i])
                }
            }
        }
        println("找到" + ints.size + "个标志轮廓!")


        val point: Array<Point?> = convertPoints(points)


        // cv::boundingRect(InputArray points)得到每一个轮廓周围最小矩形左上角点坐标和右下角点坐标
        // cv::AreaRect(InputArray points)得到一个旋转矩形，返回旋转矩形

        //轮廓转换成最小矩形包围盒
        /*val rotatedRect = minAreaRect(MatOfPoint2f(*point))
        //截取出二维码
        val qrRect = rotatedRect.boundingRect()
        val qrCodeImg = Mat(image, qrRect)
        val binaryBitmap1 =
            Bitmap.createBitmap(qrCodeImg.cols(), qrCodeImg.rows(), bitmap.config)

        Utils.matToBitmap(qrCodeImg, binaryBitmap1)*/

        runOnUiThread {
           // binding.img.setImageBitmap(binaryBitmap1)
        }

        image.release()
    }

    private fun convertPoints(points: List<MatOfPoint>): Array<Point?> {
        if (points.size != 3) {
            return arrayOfNulls<Point>(0)
        }

        val points1 = points[0].toArray()
        val points2 = points[1].toArray()
        val points3 = points[2].toArray()
        val point: Array<Point?> = arrayOfNulls<Point>(points1.size + points2.size + points3.size)
        System.arraycopy(points1, 0, point, 0, points1.size)
        System.arraycopy(points2, 0, point, points1.size, points2.size)
        System.arraycopy(points3, 0, point, points1.size + points2.size, points3.size)
        return point
    }
}