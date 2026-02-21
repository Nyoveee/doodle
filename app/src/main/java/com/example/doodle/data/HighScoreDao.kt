package com.example.doodle.data

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow

@Dao
interface HighScoreDao {

    @Query("SELECT * FROM high_scores ORDER BY score DESC LIMIT 20")
    fun getTopScores(): Flow<List<HighScoreEntity>>

    @Insert
    suspend fun insert(score: HighScoreEntity)
}
