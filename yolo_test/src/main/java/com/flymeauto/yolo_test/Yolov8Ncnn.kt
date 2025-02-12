package com.flymeauto.yolo_test

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.view.Surface
import com.flymeauto.yolo_test.TrainDetectedObject
import java.util.ArrayList
import java.util.HashMap


class Yolov8Ncnn {
    external fun loadModel(mgr: AssetManager?, modelid: Int, cpugpu: Int): Boolean
    external fun openCamera(facing: Int): Boolean
    external fun closeCamera(): Boolean
    external fun setOutputWindow(surface: Surface?): Boolean
    external fun setOutputWindow1(surface: Surface?): Boolean
    external fun detectedStaticPic(bitmap: Bitmap?): List<HashMap<String?, String?>?>?

    companion object {
        init {
            System.loadLibrary("yolov8ncnn")
        }
    }

    private val colors = intArrayOf(
        Color.rgb(54, 67, 244),
        Color.rgb(99, 30, 233),
        Color.rgb(176, 39, 156),
        Color.rgb(183, 58, 103),
        Color.rgb(181, 81, 63),
        Color.rgb(243, 150, 33),
        Color.rgb(244, 169, 3),
        Color.rgb(212, 188, 0),
        Color.rgb(136, 150, 0),
        Color.rgb(80, 175, 76),
        Color.rgb(74, 195, 139),
        Color.rgb(57, 220, 205),
        Color.rgb(59, 235, 255),
        Color.rgb(7, 193, 255),
        Color.rgb(0, 152, 255),
        Color.rgb(34, 87, 255),
        Color.rgb(72, 85, 121),
        Color.rgb(158, 158, 158),
        Color.rgb(139, 125, 96)
    )
    fun trainDataToDetectedObject(data: List<HashMap<String?, String>>?): List<TrainDetectedObject> {
        if (data == null || data.size == 0) {
            return ArrayList<TrainDetectedObject>()
        }
        val result: MutableList<TrainDetectedObject> = ArrayList<TrainDetectedObject>()
        for (i in data.indices) {
            val item = data[i]
            if (item.containsKey("label") && item.containsKey("label_str") &&
                item.containsKey("prob") && item.containsKey("x") &&
                item.containsKey("y") && item.containsKey("w") &&
                item.containsKey("h")
            ) {
                val obj: TrainDetectedObject = TrainDetectedObject()
                obj.label = item["label"]
                obj.labelName = item["label_str"]
                obj.prob = item["prob"]!!.toFloat()
                obj.x = item["x"]!!.toFloat()
                obj.y = item["y"]!!.toFloat()
                obj.w = item["w"]!!.toFloat()
                obj.h = item["h"]!!.toFloat()
                result.add(obj)
            }
        }
        return result
    }
    fun showObjects(objects: List<TrainDetectedObject>?, orgBitmap: Bitmap): Bitmap? {
        if (objects == null) {
            return null
        }
        // draw objects on bitmap
        val rgba = orgBitmap.copy(Bitmap.Config.ARGB_8888, true)

        val canvas = Canvas(rgba)

        val paint = Paint()
        paint.style = Paint.Style.STROKE
        paint.strokeWidth = 4f

        val textbgpaint = Paint()
        textbgpaint.color = Color.WHITE
        textbgpaint.style = Paint.Style.FILL

        val textpaint = Paint()
        textpaint.color = Color.BLACK
        textpaint.textSize = 26f
        textpaint.textAlign = Paint.Align.LEFT

        for (i in objects.indices) {
            paint.color = colors.get(i % 19)
            canvas.drawRect(
                objects[i].x,
                objects[i].y,
                objects[i].x + objects[i].w,
                objects[i].y + objects[i].h,
                paint
            )
            run {
                val text: String = objects[i].labelName + " = " + java.lang.String.format(
                    "%.1f",
                    objects[i].prob * 100
                ) + "%"
                val text_width = textpaint.measureText(text)
                val text_height = -textpaint.ascent() + textpaint.descent()

                var x: Float = objects[i].x
                var y: Float = objects[i].y - text_height
                if (y < 0) y = 0f
                if (x + text_width > rgba.width) x = rgba.width - text_width

                canvas.drawRect(x, y, x + text_width, y + text_height, textbgpaint)
                canvas.drawText(text, x, y - textpaint.ascent(), textpaint)
            }
        }
        return rgba
    }
}
