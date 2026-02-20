package com.example.doodle.data

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow

@Dao
interface HighScoreDao {

    // Reactive top-20 leaderboard — reemits automatically on any change
    @Query("SELECT * FROM high_scores ORDER BY highScore DESC LIMIT 20")
    fun getTopScores(): Flow<List<HighScoreEntity>>

    @Query("SELECT * FROM high_scores WHERE userId = :userId")
    fun getPlayerScore(userId: String): Flow<HighScoreEntity?>

    // REPLACE handles both insert and update atomically
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun upsert(score: HighScoreEntity)
}