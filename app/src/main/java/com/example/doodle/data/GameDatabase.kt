package com.example.doodle.data

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase

@Database(entities = [HighScoreEntity::class], version = 2, exportSchema = false)
abstract class GameDatabase : RoomDatabase() {
    abstract fun highScoreDao(): HighScoreDao

    companion object {
        @Volatile private var INSTANCE: GameDatabase? = null

        fun getInstance(context: Context): GameDatabase =
            INSTANCE ?: synchronized(this) {
                Room.databaseBuilder(
                    context.applicationContext,
                    GameDatabase::class.java,
                    "game.db"
                )
                    .fallbackToDestructiveMigration()
                    .build()
                    .also { INSTANCE = it }
            }
    }
}