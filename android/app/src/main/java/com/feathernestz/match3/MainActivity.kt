package com.feathernestz.match3

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Load native library
        try {
            System.loadLibrary("Match3")
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
        }
    }
}
