package com.example.doodle.data

import androidx.room.Entity
import androidx.room.Index
import androidx.room.PrimaryKey

@Entity(
    tableName = "high_scores",
    indices = [Index(value = ["score"])]
)
data class HighScoreEntity(
    @PrimaryKey(autoGenerate = true) val id: Int = 0,  // Auto row ID
    val userId: String,       // Internal: ties runs to the same device install
    val gameId: String,       // Internal: unique session UUID
    val username: String,     // Display name
    val score: Int,           // Score for THIS run
    val achievedAt: Long = System.currentTimeMillis()
)

