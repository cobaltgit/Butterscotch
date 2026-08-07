package net.perfectdreams.butterscotch.android.screens

import android.content.Intent
import android.net.Uri
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.unit.dp
import androidx.navigation.NavHostController
import net.perfectdreams.butterscotch.android.R
import net.perfectdreams.butterscotch.android.Route
import net.perfectdreams.butterscotch.android.components.ButterscotchBackButton
import net.perfectdreams.butterscotch.android.components.ButterscotchBobImage
import net.perfectdreams.butterscotch.android.components.ButterscotchTopBar

@Composable
fun AboutScreen(nav: NavHostController) {
    val context = LocalContext.current

    Scaffold(
        topBar = {
            ButterscotchTopBar(
                { Text("About") },
                nav,
                navigationIcon = { ButterscotchBackButton(nav) }
            )
        }
    ) { innerPadding ->

        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
                .verticalScroll(rememberScrollState())
                .padding(24.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {

            ButterscotchBobImage(
                R.drawable.butterscotch_logo,
                "Butterscotch logo"
            )

            Text(
                "Spaghetti",
                style = MaterialTheme.typography.headlineMedium
            )

            Spacer(Modifier.height(4.dp))

            Text(
                "\"Finally... Droidtale 3 - Now more than GameMaker\"",
                style = MaterialTheme.typography.headlineSmall.copy(
                    fontStyle = FontStyle.Italic
                )
            )

            Spacer(Modifier.height(16.dp))

            Text(
                "Created by MrPowerGamerBR\nFork maintained by Migi64",
                style = MaterialTheme.typography.bodyLarge
            )

            Spacer(Modifier.height(24.dp))

            Text(
                "Contributors",
                style = MaterialTheme.typography.titleMedium
            )

            Spacer(Modifier.height(8.dp))

            OutlinedButton(
                onClick = {
                    val intent = Intent(
                        Intent.ACTION_VIEW,
                        Uri.parse("https://github.com/ButterscotchRunner/Butterscotch/graphs/contributors")
                    )
                    context.startActivity(intent)
                }
            ) {
                Text("View Contributors on GitHub")
            }

            Spacer(Modifier.height(24.dp))

            OutlinedButton(
                onClick = {
                    nav.navigate(Route.Licenses)
                }
            ) {
                Text("Open-Source Licenses")
            }
        }
    }
}
